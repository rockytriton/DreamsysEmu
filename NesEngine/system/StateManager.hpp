//
//  StateManager.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/15/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef StateManager_hpp
#define StateManager_hpp

#include "Cartridge.hpp"

namespace nes::cpu {
    class Cpu;
}

namespace nes::ppu {
    class Ppu;
}

namespace nes::system {
    
    class StateManager {
    public:
        StateManager(common::Cartridge &cart);
        
        void saveState();
        void loadState();
        
        void saveBattery();
        void loadBattery();
        
        void setState(int s) { currentState = s; }
        int getState() { return currentState; }
        
        void setCpu(cpu::Cpu *p) { cpu = p; }
        void setPpu(ppu::Ppu *p) { ppu = p; }
        
    private:
        string getFileName(const string &ext);
        
        string statePath;
        string batteryPath;
        common::Cartridge &cart;
        
        cpu::Cpu *cpu;
        ppu::Ppu *ppu;
        
        int currentState = 0;
    };
    
}

#endif /* StateManager_hpp */
