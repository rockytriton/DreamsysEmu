//
//  DreamsysEmulator.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef DreamsysEmulator_hpp
#define DreamsysEmulator_hpp

#include "common.h"
#include "Cpu.hpp"
#include "Cartridge.hpp"
#include "Ppu.hpp"
#include "StateManager.hpp"

using namespace nes::common;
using namespace nes::cpu;
using namespace nes::ppu;

namespace nes::system {
    
    class DreamsysEmulator {
    public:
        DreamsysEmulator() : cart(stateManager), stateManager(cart) { stateManager.setCpu(&cpu); stateManager.setPpu(&ppu); }
        bool loadRom(const string &fileName);
        void run();
        void run2();
        
        bool isRunning() { return running; }
        
        nes::ppu::PpuData *getPpuData() {
            return &ppu.data();
        }
        
        static void updateMirroring(void *p);
        
        void powerOff() {
            paused = false;
            running = false;
        }
        
        void readAudioBuffer(float **p, Word *n);
        float *readSamples(Word *n);
        
        Byte read(Address addr) {
            return cpu.bus().read(addr);
        }
        void resetBuffer();
        
        Byte *readRam() {
            return cpu.bus().readRam();
        }
        
        void saveState() { stateManager.saveState(); }
        void loadState() { stateManager.loadState(); }
        
        
        
    private:
        Cartridge cart;
        Cpu cpu;
        Ppu ppu;
        StateManager stateManager;
        
        bool paused;
        bool stepping;
        bool running;
    };
    
}

#endif /* DreamsysEmulator_hpp */
