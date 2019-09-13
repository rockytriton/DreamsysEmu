//
//  Apu.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Apu.hpp"
#include "Cpu.hpp"

#include <mutex>
#include <condition_variable>

#include <unistd.h>

std::mutex mtx;             // mutex for critical section
std::mutex mtx2;             // mutex for critical section
std::condition_variable cv; // condition variable for critical section
std::condition_variable cv2; // condition variable for critical section

using namespace nes::apu;
Apu::Apu() {
    channels.push_back(new PulseChannel(1));
    channels.push_back(new PulseChannel(2));
    channels.push_back(new TriangleChannel());
    channels.push_back(new NoiseChannel());
    //channels.push_back(new DmcChannel());
    
    filters.push_back(new HiPassFilter(90, SAMPLE_RATE));
    filters.push_back(new HiPassFilter(440, SAMPLE_RATE));
    filters.push_back(new LoPassFilter(14000, SAMPLE_RATE));
}

Apu::~Apu() {
    for (int i=0; i<channels.size(); i++) {
        delete channels[i];
    }
    
    for (int i=0; i<filters.size(); i++) {
        delete filters[i];
    }
}

void Apu::reset() {
    for (int i=0; i<channels.size(); i++) {
        channels[i]->silence();
    }
}

Byte Apu::read(Address address) {
    if (address == 0x4015) {
        //status register.
        Byte shift = 0;
        Byte value = 0;
        for (int i=0; i<channels.size() - 1; i++) {
            value |= (channels[i]->counter() > 0 ? (1 << shift) : 0);
            shift++;
        }
        
        if (regStatus & FrameInterrupt) {
            value |= FrameInterrupt;
        }
        
        return value;
        
    } else if (BETWEEN(address, 0x4000, 0x4013)) {
        //pulse channels
        Byte channel = address & 0x1C;
        Byte reg = address & 0x3;
        if (channel < channels.size())
            return channels[channel]->read(reg);
    }
    
    return 0;
}

void Apu::write(Address address, Byte value) {
    
    if (address == 0x4015) {
        //status register.
        Byte shift = 0;
        for (int i=0; i<channels.size() - 1; i++) {
            if ((value & (1 << shift)) == 0) {
                channels[i]->silence();
            }
            value |= (channels[i]->counter() > 0 ? (1 << shift) : 0);
            shift++;
        }
        
        value &= ~DmcInterrupt;
        
        regStatus = value;
        
    } else if (BETWEEN(address, 0x4000, 0x4013)) {
        //pulse channels
        Byte channel = (address & 0x1C) >> 2;
        Byte reg = address & 0x3;
        
        if (channel < channels.size())
            channels[channel]->write(reg, value);
    } else if (address == 0x4017) {
        handleFrameCounter(value);
    }
}

void Apu::handleFrameCounter(Byte value) {
    regFrameCounter = value;
}

bool r = false;

void Apu::clockTick() {
    cycles++;
    
    
    for (int i=0; i<channels.size(); i++) {
        channels[i]->tick(cycles % 2 == 0);
    }

    if (cycles % (clockRate / 240) == 0) {
        if (regFrameCounter & Mode1) {
            
            if (frameCount == 0 || frameCount == 2) {
                for (int i=0; i<channels.size(); i++) {
                    channels[i]->sweep();
                }
            }
            
            if (frameCount != 4) {
                
                for (int i=0; i<channels.size(); i++) {
                    channels[i]->envelope();
                }
            }
            
            if (frameCount == 4) {
                frameCount = -1;
            }
            
        } else {
            for (int i=0; i<channels.size(); i++) {
                channels[i]->envelope();
            }
            
            if (frameCount == 3 && !(regFrameCounter & DisableIrq)) {
                cpu->setInterrupt(cpu::IRQ);
            }
            
            if (frameCount == 1 || frameCount == 3) {
                for (int i=0; i<channels.size(); i++) {
                    channels[i]->sweep();
                }
            }
            
        }
        
        frameCount++;
    }
    
    if (cycles == nextClock) {
        r = !r;
        nextClock += (clockRate / sampleRate) + r;
        
        if (audioIndex < 4095) {
            float sample = mixer.sample(channels[0]->output(), channels[1]->output(), channels[2]->output(), channels[3]->output(), 0);
            
            for (int i=0; i<filters.size(); i++) {
                sample = filters[i]->process(sample);
            }
            audioBuffer[audioIndex] = sample;
            
            audioIndex++;
        }
        
    }
    /*
    if (cycles % ((clockRate / sampleRate)) == 0) {
        //printf("OUT\r\n");
        //r = !r;
        float sample = mixer.sample(channels[0]->output(), channels[1]->output(), 0, 0, 0);
        //float sample = mixer.sample(channels[0]->output(), channels[1]->output(), channels[2]->output(), channels[3]->output(), 0);
        //float sample = mixer.sample(0, 0, channels[2]->output(), channels[3]->output(), 0);
        //float sample = mixer.sample(0, 0, channels[2]->output(), 0, 0);
        
        for (int i=0; i<filters.size(); i++) {
            //sample = filters[i]->process(sample);
        }
        
        std::unique_lock<std::mutex> lck(mtx);
        audioBuffer[audioIndex] = sample;
        
        if (audioIndex < 4095) {
            audioIndex++;
        } else {
            printf("PACKET LOSS!\r\n");
        }
        cv.notify_all();
    }*/
}

float copyBuffer[4096] = {0};

void Apu::resetBuffer() {
    audioIndex = 0;
}

float *Apu::readSamples(Word &len) {
    
    len = audioIndex;
    //memcpy(copyBuffer, audioBuffer, len * sizeof(float));
    //audioIndex = 0;
    return audioBuffer;
    //return copyBuffer;
}


float *Apu::readSamples2(Word &len) {
    int totalLen = 0;
    
    if (audioIndex == 0) {
        len = 0;
        return copyBuffer;
    }
    
    int tries = 0;
    
    while(totalLen <= 2000) {
        int readLen = audioIndex;
        
        if (readLen == 0) {
            if (++tries > 0) {
                break;
            }
            usleep(10);
            //cv.notify_all();
            continue;
        }
        
        std::unique_lock<std::mutex> lck(mtx);
        
        if (readLen + totalLen > 4000) {
            int diff = readLen + totalLen - 4000;
            
            memcpy(copyBuffer + totalLen, audioBuffer, (readLen - diff));
            totalLen += (readLen - diff);
            
            memcpy(audioBuffer, audioBuffer + readLen - diff, diff);
            audioIndex = diff;
            cv.notify_all();
            break;
        }
        
        memcpy(copyBuffer + (totalLen * sizeof(float)), audioBuffer, readLen * sizeof(float));
        
        totalLen += readLen;
        audioIndex = 0;
        cv.notify_all();
        
        //usleep(1);
    }
    
    len = totalLen;
    //audioIndex = 0;
    return copyBuffer;
}

void Apu::readAudio(float *&samples, Word &len) {
    samples = audioBuffer;
    len = audioIndex;
    audioIndex = 0;
}

Mixer::Mixer() {
    for (Word i = 0; i < 31;  i++) this->pulseTable[i] = 95.52  / (8128.0  / float(i) + 100);
    for (Word i = 0; i < 203; i++) this->tndTable[i]   = 163.67 / (24329.0 / float(i) + 100);
}

float Mixer::sample(Byte pulse1, Byte pulse2, Byte triangle, Byte noise, Byte dmc) {
    return pulseTable[pulse1 + pulse2] + tndTable[3 * triangle + 2 * noise + dmc];
}


extern "C" void emu_done_read();

void emu_done_read() {
    //cv.notify_all();
}
