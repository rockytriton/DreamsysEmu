//
//  MemoryAddressing.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "MemoryAddressor.hpp"

#include "Cpu.hpp"
#include "Log.hpp"

using namespace nes::cpu;


MemoryAddressor::MemoryAddressor() {
    
}

Byte MemoryAddressor::fetchAbsAddr(Cpu &cpu, Byte increment) {
    Address addr = cpu.bus().readWord(cpu.data().pc);
    cpu.data().absoluteAddr = addr + increment;
    cpu.data().pc += 2;
    
    if ((cpu.data().absoluteAddr & 0xFF00) != (addr & 0xFF00)) {
        return 1;
    }
    
    return 0;
}

Byte MemoryAddressor::fetch(Cpu &cpu, OpCode &opCode) {
    Address addr = 0;
    
    switch(opCode.addressMode) {
        case IMP:
            cpu.data().fetched = cpu.data().regA;
            return 0;
            
        case ACC:
            cpu.data().fetched = cpu.data().regA;
            return 0;
            
        case IMM:
            cpu.data().absoluteAddr = cpu.data().pc;
            cpu.data().pc++;
            return 0;
            
        case ZP:
            cpu.data().absoluteAddr = cpu.bus().read(cpu.data().pc);
            cpu.data().absoluteAddr &= 0x00FF;
            cpu.data().pc++;
            return 0;
            
        case ZPX:
            cpu.data().absoluteAddr = cpu.bus().read(cpu.data().pc) + cpu.data().regX;
            cpu.data().absoluteAddr &= 0x00FF;
            cpu.data().pc++;
            return 0;
            
        case ZPY:
            cpu.data().absoluteAddr = cpu.bus().read(cpu.data().pc) + cpu.data().regY;
            cpu.data().absoluteAddr &= 0x00FF;
            cpu.data().pc++;
            return 0;
            
        case ABS:
            return fetchAbsAddr(cpu, 0);
            
        case ABSX:
            return fetchAbsAddr(cpu, cpu.data().regX);
            
        case ABSY:
            return fetchAbsAddr(cpu, cpu.data().regY);
            
        case IND:
            addr = cpu.bus().readWord(cpu.data().pc);
            
            if ((addr & 0xFF) == 0xFF) {
                cpu.data().absoluteAddr = (cpu.bus().read(addr & 0xFF00) << 8) | cpu.bus().read(addr);
            } else {
                cpu.data().absoluteAddr = (cpu.bus().read(addr + 1) << 8) | cpu.bus().read(addr);
            }
            
            cpu.data().pc += 2;
            return 0;
            
        case INDX:
            addr = cpu.bus().read(cpu.data().pc);
            cpu.data().pc++;
            cpu.data().absoluteAddr = cpu.bus().readWord(addr, cpu.data().regX);
            return 0;
            
        case INDY:
            addr = cpu.bus().read(cpu.data().pc);
            cpu.data().pc++;
            cpu.data().absoluteAddr = cpu.bus().readWord(addr, 0);
            cpu.data().absoluteAddr += cpu.data().regY;
            
            if ((cpu.data().absoluteAddr & 0xFF00) != ((cpu.data().absoluteAddr - cpu.data().regY) & 0xFF00)) {
                return 1;
            }
            
            return 0;
            
        case REL:
            cpu.data().relativeAddr = cpu.bus().read(cpu.data().pc);
            cpu.data().pc++;
            
            if (cpu.data().relativeAddr & 0x80) {
                cpu.data().relativeAddr |= 0xFF00;
            }
            
            return 0;
    }
    
    return 0;
}

