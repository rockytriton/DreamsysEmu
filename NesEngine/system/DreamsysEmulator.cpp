//
//  DreamsysEmulator.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "DreamsysEmulator.hpp"
#include "Log.hpp"
#include "Timer.hpp"
#include "CpuBus.hpp"

#include <unistd.h>

using namespace nes::system;

void DreamsysEmulator::updateMirroring(void *p) {
    PpuBus *bus = (PpuBus *)p;
    
    bus->updateMirroring();
}

bool DreamsysEmulator::loadRom(const string &fileName) {
    if (!cart.load(fileName)) {
        return false;
    }
    
    cart.getMapper().setCpu(&cpu);
    cart.getMapper().setPpu(&ppu);
    
    paused = false;
    running = true;
    stepping = false;
    
    cpu.bus().setMapper(&cart.getMapper());
    cpu.bus().setPpu(&ppu);
    cpu.reset();
    
    ppu.bus().setMapper(&cart.getMapper());
    ppu.processor().setCpu(&cpu);
    
    ppu.reset();
    
    cart.getMapper().setMirroringCallback(updateMirroring, (void *)&ppu.bus());
    
    return true;
}

void DreamsysEmulator::run() {
    Timer timer;
    LOG << "Running" << endl;
    
    while(running) {
        
        if (paused) {
            if (stepping) {
                stepping = false;
            } else {
                usleep(1500);
                continue;
            }
        }
        
        timer.update();
        
        while(timer.loop()) {
            ppu.clockTick();
            ppu.clockTick();
            ppu.clockTick();
            cpu.clockTick();
            cpu.bus().getApu()->clockTick();
            
            timer.cycle();
            
            if (paused) {
                break;
            }
        }
        
        //usleep(1);
    }
}

float *p = new float[4096];

void DreamsysEmulator::readAudioBuffer(float **pp, Word *n) {
    
    cpu.bus().getApu()->readAudio(*pp, *n);
    //memcpy(*pp, p, *n * sizeof(float));
    /*
    for (int i=0; i<*n; i++) {
        float *fp = *pp;
        fp[i] = p[i];
    }*/
}

float *DreamsysEmulator::readSamples(Word *n) {
    return cpu.bus().getApu()->readSamples(*n);
}
