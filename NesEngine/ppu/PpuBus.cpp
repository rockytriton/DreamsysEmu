//
//  PpuBus.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "PpuBus.hpp"

using namespace nes::ppu;

PpuBus::PpuBus() {
    
}

void PpuBus::reset() {
    memset(nameTables, 0, 8);
    busRam = new Byte[0x800];
    palette = new Byte[0x20];
    
    memset(busRam, 0, 0x800);
    memset(palette, 0, 0x20);
}

bool PpuBus::saveState(std::ofstream &stream) {
    stream.write((char *)busRam, 0x800);
    stream.write((char *)palette, 0x20);
    stream.write((char *)nameTables, 8);
    
    return true;
}

bool PpuBus::loadState(std::ifstream &stream) {
    stream.read((char *)busRam, 0x800);
    stream.read((char *)palette, 0x20);
    stream.read((char *)nameTables, 8);
    
    return true;
}

Byte PpuBus::read(Address addr) {
    a12Status = ((addr >> 12) & 1);
    //Byte b = mapper->readCHR(addr);
    
    if (addr < 0x2000) {
        return mapper->readCHR(addr);
    } if (addr < 0x3EFF) {
        uint16_t index = addr & 0x3FF;
        
        if (addr < 0x2400) {
            //NT0
            return busRam[nameTables[0] + index];
        } else if (addr < 0x2800) {
            //NT1
            return busRam[nameTables[1] + index];
        } else if (addr < 0x2c00) {
            //NT1
            return busRam[nameTables[2] + index];
        } else {
            //NT1
            return busRam[nameTables[3] + index];
        }
    } else if (addr < 0x3FFF) {
        return palette[addr & 0x1F];
    }
    
    return 0;
}

void PpuBus::write(Address addr, Byte value) {
    a12Status = ((addr >> 12) & 1);
    //mapper->writeCHR(addr, value);
    
    if (addr < 0x2000) {
        mapper->writeCHR(addr, value);
        return;
    } else if (addr < 0x3EFF) {
        uint16_t index = addr & 0x3FF;
        
        if (addr < 0x2400) {
            //NT0
            busRam[nameTables[0] + index] = value;
        } else if (addr < 0x2800) {
            //NT1
            busRam[nameTables[1] + index] = value;
        } else if (addr < 0x2c00) {
            //NT1
            busRam[nameTables[2] + index] = value;
        } else {
            //NT1
            busRam[nameTables[3] + index] = value;
        }
    } else if (addr < 0x3FFF) {
        palette[addr == 0x3F10 ? 0 : addr & 0x1F] = value;
    }
}

void PpuBus::updateMirroring() {
    switch(mapper->getMirrorType()){
        case Horizontal:
            nameTables[0] = nameTables[1] = 0;
            nameTables[2] = nameTables[3] = 0x400;
            break;
        case Vertical:
            nameTables[0] = nameTables[2] = 0;
            nameTables[1] = nameTables[3] = 0x400;
            break;
        case OneScreenLower:
            nameTables[0] = nameTables[1] = 0;
            nameTables[2] = nameTables[3] = 0;
            break;
        case OneScreenHigher:
            nameTables[0] = nameTables[1] = 0x400;
            nameTables[2] = nameTables[3] = 0x400;
            break;
        default:
            nameTables[0] = nameTables[1] = 0;
            nameTables[2] = nameTables[3] = 0;
            break;
    }
}

