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

using namespace nes::common;
using namespace nes::cpu;
using namespace nes::ppu;

namespace nes::system {
    
    class DreamsysEmulator {
    public:
        bool loadRom(const string &fileName);
        void run();
        
        bool isRunning() { return running; }
        
        nes::ppu::PpuData *getPpuData() {
            return &ppu.data();
        }
        
        static void updateMirroring(void *p);
        
        void powerOff() {
            paused = false;
            running = false;
        }
        
    private:
        Cartridge cart;
        Cpu cpu;
        Ppu ppu;
        
        bool paused;
        bool stepping;
        bool running;
    };
    
}

#endif /* DreamsysEmulator_hpp */
