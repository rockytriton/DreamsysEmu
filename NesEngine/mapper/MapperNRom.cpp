//
//  MapperNRom.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "MapperNRom.hpp"

using namespace nes::mapper;

MapperNRom::MapperNRom(Cartridge &cart) : Mapper(cart) {
    prgRam = new Byte[64 * 1024];
}

MapperNRom::~MapperNRom() {
    delete [] prgRam;
}

void MapperNRom::init() {
    Mapper::init();
    oneBank = prgBanks.size() == 1;
}

Byte MapperNRom::readPRG(Address addr) {
    if (addr < 0x6000) {
        return 0;
    } else if (addr < 0x8000) {
        return prgRam[addr - 0x6000];
    }
    
    if (oneBank) {
        return prgBanks[0][((addr - 0x8000) & 0x3FFF)];
    }
    
    Address newAddr = addr - 0x8000;
    Address bank = newAddr / 0x4000;
    
    Byte b = prgBanks[bank % prgBanks.size()][addr % 0x4000];
    
    return b;
}

Byte MapperNRom::readCHR(Address addr) {
    if (addr >= 0x2000) {
        return 0;
    }
    
    return chrBanks[0][addr];
}

void MapperNRom::writePRG(Address addr, Byte value) {
    if (addr < 0x6000) {
        return;
    } else if (addr < 0x8000) {
        prgRam[addr - 0x6000] = value;
    }
}

void MapperNRom::writeCHR(Address addr, Byte value) {
    chrBanks[0][addr] = value;
}
