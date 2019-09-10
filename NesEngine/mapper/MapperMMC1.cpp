//
//  MapperMMC1.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/9/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "MapperMMC1.hpp"

using namespace nes::mapper;

MapperMMC1::MapperMMC1(Cartridge &cart) : Mapper(cart) {
    prgRam = new Byte[0x2000];
}

MapperMMC1::~MapperMMC1() {
    delete [] prgRam;
}

void MapperMMC1::init() {
    Mapper::init();
    
    if (cart.getHeader().chrSize) {
        mmc1ChrBanks[0] = chrBanks[0];
        mmc1ChrBanks[1] = chrBanks[0];
    }
    
    mmc1PrgBanks[0] = prgBanks[0];
    mmc1PrgBanks[1] = prgBanks[prgBanks.size() - 1];
}

Byte MapperMMC1::readPRG(Address addr) {
    if (addr < 0x8000) {
        return prgRam[addr - 0x6000];
    }
    
    if (addr < 0xC000) {
        return mmc1PrgBanks[0][addr - 0x8000];
    }
    
    return mmc1PrgBanks[1][addr - 0xC000];
}

Byte MapperMMC1::readCHR(Address addr) {
    if (addr < 0x1000) {
        return mmc1ChrBanks[0][addr];
    }
    
    return mmc1ChrBanks[1][addr - 0x1000];
}

bool MapperMMC1::shiftWrite(Address addr, Byte value) {
    bool shiftDone = regLoad & 1;
    
    regLoad >>= 1;
    regLoad |= (value & 1) << 4;
    
    if (!shiftDone) {
        return false;
    }
    
    return true;
}

void MapperMMC1::switchCharBanks() {
    if (chrRam && regControl & 0x10) {
        mmc1ChrBanks[0] = &chrRam[regChrBank0 % 2];
        mmc1ChrBanks[1] = &chrRam[regChrBank1 % 2];
    } else if (chrRam) {
        Byte bank0 = (regChrBank0 & 0x1E) % 2;
        mmc1ChrBanks[0] = &chrRam[bank0];
        mmc1ChrBanks[1] = &chrRam[(bank0 + 1) % 2];
    } else if (regControl & 0x10) {
        mmc1ChrBanks[0] = chrBanks[regChrBank0 % chrBanks.size()];
        mmc1ChrBanks[1] = chrBanks[regChrBank1 % chrBanks.size()];
    } else {
        Byte bank0 = (regChrBank0 & 0x1E) % chrBanks.size();
        mmc1ChrBanks[0] = chrBanks[bank0];
        mmc1ChrBanks[1] = chrBanks[(bank0 + 1) % chrBanks.size()];
        
        return;
    }
}

void MapperMMC1::switchPrgBanks() {
    Byte mode = (regControl >> 2) & 0x3;
    
    if (mode <= 1) {
        //32kb switch mode
        mmc1PrgBanks[0] = prgBanks[(regPrgBank & 0x0E)  % prgBanks.size()];
        mmc1PrgBanks[1] = prgBanks[((regPrgBank & 0x0E) + 1)  % prgBanks.size()];
    } else if (mode == 2) {
        //fix first, switch second
        mmc1PrgBanks[0] = prgBanks[0];
        mmc1PrgBanks[1] = prgBanks[(regPrgBank) % prgBanks.size()];
    } else if (mode == 3) {
        //switch first, fix second
        mmc1PrgBanks[0] = prgBanks[(regPrgBank) % prgBanks.size()];
        mmc1PrgBanks[1] = prgBanks[prgBanks.size() - 1];
    }
    
    switchCharBanks();
}

void MapperMMC1::writePRG(Address addr, Byte value) {
    if (addr >= 0x4020 && addr < 0x6000) {
        return; //nothing, exp rom
    }
    
    if (addr < 0x8000) {
        //chr ram
        prgRam[addr - 0x6000] = value;
        return;
    }
    
    if (value & 0x80) {
        regLoad = 0x10;
        regControl |= 0x0C;
        return;
    }
    
    if (!shiftWrite(addr, value)) {
        return;
    }
    
    if (addr <= 0x9FFF) {
        regControl = regLoad;
        
        switch(regControl & 0x03) {
            case 0: mirrorType = OneScreenLower; break;
            case 1: mirrorType = OneScreenHigher; break;
            case 2: mirrorType = Vertical; break;
            case 3: mirrorType = Horizontal; break;
        }
        
        mirroringCallback(cbVal);
        switchCharBanks();
        switchPrgBanks();
        
    } else if (addr <= 0xBFFF) {
        regChrBank0 = regLoad;
        switchCharBanks();
    } else if (addr <= 0xDFFF) {
        regChrBank1 = regLoad;
        switchCharBanks();
    } else {
        regPrgBank = regLoad;
        switchPrgBanks();
    }
    
    regLoad = 0x10;
}

void MapperMMC1::writeCHR(Address addr, Byte value) {
    if (addr < 0x1000) {
        mmc1ChrBanks[0][addr] = value;
    }
    
    mmc1ChrBanks[1][addr - 0x1000] = value;
}
