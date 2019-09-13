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
#include <thread>
#include <iostream>

using namespace nes::system;

const double targetFPS = 60.0988; //NTSC Vertical Scan Rate

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

bool timeSync = true;
extern "C" void updateAudio();

void DreamsysEmulator::run() {
    LOG << "Running" << endl;

    double targetFrameLength = 1000.0/targetFPS;
    // Record start time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int frameCount = 0;
    int droppedFrames = 0;
    int sleptFrames = 0;
    double totalFrames = 0;
    double totalTime = 0;
    auto frameSetStart = std::chrono::high_resolution_clock::now();
    
    while(running) {
        bool dropFrame = false;
        bool sleepFrame = false;
        
        std::chrono::duration<double> realTime = std::chrono::high_resolution_clock::now() - startTime; //How much time we've taken in the real world
        double gameTime = (frameCount * targetFrameLength);   //How much time in the game has passed
        double diff1 = /*gameTime -*/ (realTime.count() * 1000.0);
        
        if (diff1 < targetFrameLength) sleepFrame = true; //We are behind. Sleep a frame.
        if ((realTime.count() * 1000.0) - gameTime > targetFrameLength) dropFrame = true; //We are ahead. Skip rendeirng.
        
        //printf("RT: %f, DIFF1: %f, DIFF2: %f\r\n", realTime.count() * 1000.0, diff1, (realTime.count() * 1000.0) - gameTime);
        
        
        if (!paused) {
            if (sleepFrame == true && timeSync == true) {
                long sleepTime =(long)(targetFrameLength - diff1) / 2;
                std::this_thread::sleep_for(std::chrono::milliseconds(6));
                //updateAudio();
                //std::this_thread::sleep_for(std::chrono::milliseconds(6));
                sleptFrames++;
                //printf("SLEEP %ld\r\n", sleepTime);
            }
            
            auto frameStart = std::chrono::high_resolution_clock::now();
            int currentFrame = getPpuData()->curFrame;
            while(getPpuData()->curFrame == currentFrame) {
                ppu.clockTick();
                ppu.clockTick();
                ppu.clockTick();
                cpu.clockTick();
                //cpu.bus().getApuNew()->step();
                cpu.bus().getApu()->clockTick();
            }
            auto frameEnd = std::chrono::high_resolution_clock::now();
            auto diff = frameEnd - frameStart;
            double diffc = diff.count() / 1000000.0;
            diffc /= 1000.0;
            
            totalTime += diffc;
            totalFrames++;
            
            if (((int)(totalFrames) % 100) == 0) {
                std::cout << "AVG Frame Time: " <<  (totalTime / totalFrames) << endl;
            }
            
            if (dropFrame == true && timeSync == true){
                //printf("Skipping frame\n");
                droppedFrames++;
            } else {
#ifdef USE_PORT_AUDIO
                updateAudio();
#endif
            }
            frameCount++;
            
            if ((frameCount % 100) == 0) {
                printf("FRAM: %u\r\n", frameCount);
            }
            
            startTime = std::chrono::high_resolution_clock::now();
            
            if (frameCount > targetFPS) {
                auto frameSetEnd = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> d = frameSetEnd - frameSetStart;
                
                std::cout << "Frames: " << std::dec << frameCount << " in " << ((double)(d.count())) << " seconds" << endl;
                
                frameCount = 0;
                
                frameSetStart = std::chrono::high_resolution_clock::now();
            }
        } else {
            startTime = std::chrono::high_resolution_clock::now();
            frameCount = 0;
            usleep(50000);
        }
    }
    
}

void DreamsysEmulator::resetBuffer() {
    //cpu.bus().getApuNew()->resetBuffer();
    cpu.bus().getApu()->resetBuffer();
}

void DreamsysEmulator::run2() {
    Timer timer;
    LOG << "Running" << endl;
    
    int currentFrame = 0;
    
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
            //cpu.bus().getApuNew()->step();
            //cpu.bus().getApu()->clockTick();
            
            
            if (getPpuData()->curFrame != currentFrame) {
                currentFrame = getPpuData()->curFrame;
                updateAudio();
            }
            
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
    
    //cpu.bus().getApu()->readAudio(*pp, *n);
    //memcpy(*pp, p, *n * sizeof(float));
    /*
    for (int i=0; i<*n; i++) {
        float *fp = *pp;
        fp[i] = p[i];
    }*/
}

extern unsigned int audioBufferLength;
extern float audioBuffer[AUDIO_BUFFER_SIZE];

float *DreamsysEmulator::readSamples(Word *n) {
    //*n = audioBufferLength;
    //return audioBuffer;
    return cpu.bus().getApu()->readSamples(*n);
}

