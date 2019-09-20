//
//  PpuV2.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/18/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "PpuV2.hpp"

using namespace nes::ppu;

void PpuV2::reset() {
    state.oamData = new Byte[0x100];
}

Byte PpuV2::readRegister(PpuRegisters reg) {
    switch(reg) {
        case PpuStatus: {
            if (state.scanLine == 241) {
                if (state.cycle == 0) {
                    state.nmiSuppressed = true;
                } else if (state.cycle == 1 || state.cycle == 2) {
                    state.nmiSuppressed = true;
                    state.statusRegister.flag.vBlank = true;
                    //TODO: CPU REQUEST NMI
                }
            }
            
            Byte status = state.statusRegister.reg;
            state.statusRegister.flag.vBlank = false;
            state.writeLatch = false;
            
            return status;
        }
            
        case OAMData: {
            return state.oamData[state.oamAddress];
        }
            
        case PpuData: {
            Address address = state.vramAddress;
            state.vramAddress += state.controlRegister.flag.incMode ? 32 : 1;
            
        }
            
        default: break;
    }
    
    return 0;
}

void PpuV2::writeRegister(PpuRegisters reg, Byte b) {
    switch(reg) {
            
    }
}

