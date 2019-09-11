//
//  Apu.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Apu.hpp"
#include "Cpu.hpp"

using namespace nes::apu;
Apu::Apu() {
    channels.push_back(new PulseChannel(1));
    channels.push_back(new PulseChannel(2));
    channels.push_back(new TriangleChannel());
    channels.push_back(new NoiseChannel());
    //channels.push_back(new DmcChannel());
    
    filters.push_back(new HiPassFilter(90, 96000));
    filters.push_back(new HiPassFilter(440, 96000));
    filters.push_back(new LoPassFilter(14000, 96000));
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
            
        } else {
            if (frameCount == 3 && !(regFrameCounter & DisableIrq)) {
                cpu->setInterrupt(cpu::IRQ);
            }
            
            if (frameCount == 1 || frameCount == 3) {
                for (int i=0; i<channels.size(); i++) {
                    channels[i]->sweep();
                }
            }
            for (int i=0; i<channels.size(); i++) {
                channels[i]->envelope();
            }
            
        }
        
        frameCount++;
    }
    
    
    if (cycles % (clockRate / sampleRate) == 0) {
        float sample = mixer.sample(channels[0]->output(), channels[1]->output(), channels[2]->output(), channels[3]->output(), 0);
        //float sample = mixer.sample(0, 0, 0, channels[3]->output(), 0);
        //float sample = mixer.sample(0, 0, channels[2]->output(), 0, 0);
        
        for (int i=0; i<filters.size(); i++) {
            sample = filters[i]->process(sample);
        }
        
        audioBuffer[audioIndex] = sample;
        
        if (audioIndex < 4095) {
            audioIndex++;
        } else {
            printf("PACKET LOSS!\r\n");
        }
    }
}

float *Apu::readSamples(Word &len) {
    len = audioIndex;
    audioIndex = 0;
    return audioBuffer;
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
