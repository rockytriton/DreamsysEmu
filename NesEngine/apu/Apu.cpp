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
    channelsV.push_back(new PulseChannel(1));
    channelsV.push_back(new PulseChannel(2));
    channelsV.push_back(new TriangleChannel());
    channelsV.push_back(new NoiseChannel());
    //channels.push_back(new DmcChannel());
    
    filters.push_back(new HiPassFilter(90, SAMPLE_RATE));
    filters.push_back(new HiPassFilter(440, SAMPLE_RATE));
    filters.push_back(new LoPassFilter(14000, SAMPLE_RATE));
    
    numChannels = (int)channelsV.size();
    
    for (int i=0; i<numChannels; i++) {
        channels[i] = channelsV[i];
    }
}

Apu::~Apu() {
    for (int i=0; i<numChannels; i++) {
        delete channels[i];
    }
    
    for (int i=0; i<filters.size(); i++) {
        delete filters[i];
    }
}

void Apu::reset() {
    for (int i=0; i<numChannels; i++) {
        channels[i]->silence();
    }
}

Byte Apu::read(Address address) {
    if (address == 0x4015) {
        //status register.
        Byte shift = 0;
        Byte value = 0;
        for (int i=0; i<numChannels - 1; i++) {
            value |= (channels[i]->counter() > 0 ? (1 << shift) : 0);
            shift++;
        }
        
        if (regStatus & FrameInterrupt) {
            value |= FrameInterrupt;
        }
        
        frameIrq = false;
        cpu->clearIrq();
        
        return value;
        
    } else if (BETWEEN(address, 0x4000, 0x4013)) {
        //pulse channels
        Byte channel = address & 0x1C;
        Byte reg = address & 0x3;
        if (channel < numChannels)
            return channels[channel]->read(reg);
    }
    
    return 0;
}

void Apu::write(Address address, Byte value) {
    
    if (address == 0x4015) {
        //status register.
        Byte shift = 0;
        for (int i=0; i<numChannels - 1; i++) {
            if ((value & (1 << shift)) == 0) {
                channels[i]->silence();
            } else {
                channels[i]->wake();
            }
            value |= (channels[i]->counter() > 0 ? (1 << shift) : 0);
            shift++;
        }
        
        value &= ~DmcInterrupt;
        
        if (value != 0) {
            value = value;
        }
        
        regStatus = value;
        cpu->clearIrq();
        
    } else if (BETWEEN(address, 0x4000, 0x4013)) {
        //pulse channels
        Byte channel = (address & 0x1C) >> 2;
        Byte reg = address & 0x3;
        
        if (channel < numChannels)
            channels[channel]->write(reg, value);
    } else if (address == 0x4017) {
        handleFrameCounter(value);
    }
}

void Apu::handleFrameCounter(Byte value) {
    regFrameCounter = value;
    countFrame = true;
}

bool r = false;

void Apu::clockTick() {
    cycles++;
    bool even = (cycles % 2) == 0;
    
    if (countFrame && even) {
        countFrame = false;
        intInhibit = ((regFrameCounter & 0x40) != 0);
        
        if (intInhibit) {
            frameIrq = false;
            cpu->clearIrq();
        }
    }
    
    for (int i=0; i<numChannels; i++) {
        channels[i]->tick(even);
    }

    if (cycles % (clockRate / 240) == 0) {
        if (regFrameCounter & Mode1) {
            
            if (frameCount == 0 || frameCount == 2) {
                for (int i=0; i<numChannels; i++) {
                    channels[i]->sweep();
                }
            }
            
            if (frameCount != 4) {
                
                for (int i=0; i<numChannels; i++) {
                    channels[i]->envelope();
                }
            }
            
            if (frameCount == 4) {
                frameCount = -1;
            }
            
        } else {
            for (int i=0; i<numChannels; i++) {
                channels[i]->envelope();
            }
            
            if (frameCount == 3 && !(regFrameCounter & DisableIrq)) {
                cpu->setInterrupt(cpu::IRQ);
            }
            
            if (frameCount == 1 || frameCount == 3) {
                for (int i=0; i<numChannels; i++) {
                    channels[i]->sweep();
                }
            }
            
        }
        
        frameCount++;
    }
    
    if (frameIrq) {
        cpu->setInterrupt(cpu::IRQ);
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
