//
//  MapperMMC1.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/9/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "MapperMMC1.hpp"

#include "Log.hpp"

#include <sys/stat.h>

using namespace nes::mapper;

static int selectedBank = 0;
char szBankLocation[255] = "/Documents/rockemu/saved-states/";
char szBatteryLocation[255] = "/Documents/rockemu/saved-battery/";

MapperMMC1::MapperMMC1(Cartridge &cart) : Mapper(cart) {
    prgRam = new Byte[0x2000];
}

MapperMMC1::~MapperMMC1() {
    delete [] prgRam;
}

void MapperMMC1::init() {
    Mapper::init();
    
    mmc1ChrBanks[0] = chrBanks[0];
    mmc1ChrBanks[1] = chrBanks[1];
    
    mmc1PrgBanks[0] = prgBanks[0];
    mmc1PrgBanks[1] = prgBanks[prgBanks.size() - 1];
    
    loadBattery();
}

sDuration saveDuration(10);

void MapperMMC1::saveBattery() {
    
    char szFile[255];
    sprintf(szFile, "%s/Documents/rockemu", getenv("HOME"));
    
    mkdir(szFile, S_IXUSR | S_IWUSR | S_IRUSR);
    printf("LOC: %s %d\r\n", szFile, errno);
    sprintf(szFile, "%s%s", getenv("HOME"), szBatteryLocation);
    mkdir(szFile, S_IXUSR | S_IWUSR | S_IRUSR);
    printf("LOC: %s %d\r\n", szFile, errno);
    
    
    const char *p = strstr(cart.getFileName().c_str(), "/");
    const char *pp = p;
    
    while((pp = strstr(p, "/")) != 0) {
        p = pp + 1;
    }
    
    sprintf(szFile, "%s%s%s.save", getenv("HOME"), szBatteryLocation, p);
    
    std::ofstream out(szFile, std::ios::binary);
    out.write((char *) prgRam, 0x2000);
    out.close();
}

void MapperMMC1::loadBattery() {
    
    
    char szFile[255];
    sprintf(szFile, "%s/Documents/rockemu", getenv("HOME"));
    
    mkdir(szFile, S_IXUSR | S_IWUSR | S_IRUSR);
    printf("LOC: %s %d\r\n", szFile, errno);
    sprintf(szFile, "%s%s", getenv("HOME"), szBatteryLocation);
    mkdir(szFile, S_IXUSR | S_IWUSR | S_IRUSR);
    printf("LOC: %s %d\r\n", szFile, errno);
    
    
    const char *p = strstr(cart.getFileName().c_str(), "/");
    const char *pp = p;
    
    while((pp = strstr(p, "/")) != 0) {
        p = pp + 1;
    }
    
    sprintf(szFile, "%s%s%s-%d.save", getenv("HOME"), szBatteryLocation, p);
    
    std::ifstream in(szFile, std::ios::binary);
    
    if (!in) {
        return;
    }
    
    in.read((char *) prgRam, 0x2000);
    in.close();
}

Byte MapperMMC1::readPRG(Address addr) {
    if (checkBatterySave) {
        hrClock::time_point now = std::chrono::high_resolution_clock::now();
        
        if ((now - lastSaveTime) > saveDuration) {
            printf("SHOULD SAVE\r\n");
            lastSaveTime = now;
            checkBatterySave = false;
            saveBattery();
        } else {
            //std::cout << "Not Yet: " << (now - lastSaveTime).count() << endl;
        }
    }
    
    if (addr < 0x8000) {
        return prgRam[addr - 0x6000];
    }
    
    if (addr < 0xC000) {
        return mmc1PrgBanks[0][addr & 0x3FFF];
    }
    
    return mmc1PrgBanks[1][addr & 0x3FFF];
}

Byte MapperMMC1::readCHR(Address addr) {
    if (chrRam != nullptr) {
        return chrRam[addr];
    }
    
    if (addr >= 0x2000) {
        return 0;
    }
    
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
        if (prgRamDisabled) {
            return;
        }
        
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
        bool prev = prgRamDisabled;
        prgRamDisabled = (value & 0x10) != 0;
        
        checkBatterySave = true;
        
        regPrgBank = regLoad;
        switchPrgBanks();
    }
    
    regLoad = 0x10;
}

void MapperMMC1::writeCHR(Address addr, Byte value) {
    if (chrRam != nullptr) {
        chrRam[addr] = value;
        return;
    }
    if (addr >= 0x2000) {
        return;
    }
    
    if (addr < 0x1000) {
        mmc1ChrBanks[0][addr] = value;
    }
    
    mmc1ChrBanks[1][addr - 0x1000] = value;
}
