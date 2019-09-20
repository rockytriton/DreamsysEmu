//
//  MapperMMC3.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/9/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "MapperMMC3.hpp"
#include "Cpu.hpp"
#include "Ppu.hpp"
#include "Log.hpp"

using namespace nes::mapper;

MapperMMC3::MapperMMC3(Cartridge &cart) : Mapper(cart) {
    
}

MapperMMC3::~MapperMMC3() {
    
}

void MapperMMC3::onSelectChr() {
    
    if (regBankSelect & 0x80) {
        //chr banks
        mmc3chrBank[0] = chrBanks[((bankValues[2]) % chrBankSize)];
        mmc3chrBank[1] = chrBanks[((bankValues[3]) % chrBankSize)];
        mmc3chrBank[2] = chrBanks[((bankValues[4]) % chrBankSize)];
        mmc3chrBank[3] = chrBanks[((bankValues[5]) % chrBankSize)];
        mmc3chrBank[4] = chrBanks[((bankValues[0]  & 0xFE) % chrBankSize)];
        mmc3chrBank[5] = chrBanks[((bankValues[0] | 0x1) % chrBankSize)];
        mmc3chrBank[6] = chrBanks[((bankValues[1]  & 0xFE) % chrBankSize)];
        mmc3chrBank[7] = chrBanks[((bankValues[1] | 0x1) % chrBankSize)];
    } else {
        //chr banks
        mmc3chrBank[0] = chrBanks[((bankValues[0]  & 0xFE) % chrBankSize)];
        mmc3chrBank[1] = chrBanks[((bankValues[0] | 0x1) % chrBankSize)];
        mmc3chrBank[2] = chrBanks[((bankValues[1]  & 0xFE) % chrBankSize)];
        mmc3chrBank[3] = chrBanks[((bankValues[1] | 0x1) % chrBankSize)];
        mmc3chrBank[4] = chrBanks[((bankValues[2]) % chrBankSize)];
        mmc3chrBank[5] = chrBanks[((bankValues[3]) % chrBankSize)];
        mmc3chrBank[6] = chrBanks[((bankValues[4]) % chrBankSize)];
        mmc3chrBank[7] = chrBanks[((bankValues[5]) % chrBankSize)];
    }
}


void MapperMMC3::onSelectPrg() {
    
    if (regBankSelect & 0x40) {
        //prg banks
        mmc3prgBank[0] = prgBanks[prgBanks.size() - 2];
        mmc3prgBank[1] = prgBanks[(bankValues[7] & 0x3F) % prgBanks.size()];
        mmc3prgBank[2] = prgBanks[(bankValues[6] & 0x3F) % prgBanks.size()];
        mmc3prgBank[3] = prgBanks[(prgBankSize - 1)];
    } else {
        //prg banks
        mmc3prgBank[0] = prgBanks[(bankValues[6] & 0x3F) % prgBanks.size()];
        mmc3prgBank[1] = prgBanks[(bankValues[7] & 0x3F) % prgBanks.size()];
        mmc3prgBank[2] = prgBanks[(prgBankSize - 2)];
        mmc3prgBank[3] = prgBanks[(prgBankSize - 1)];
    }
}

void MapperMMC3::onScanlineCount(Address address) {
    bool current = lastA12;
    lastA12 = address >= 0x1000;
    
    if (scanLineCounter == 0xc1) {
        address += 0;
    }
    
    if (address == 8183) {
        address += 0;
    }
    
    if (!current && lastA12) {
        if (scanLineCounter == 0 || regIrqReload) {
            regIrqReload = 0;
            scanLineCounter = regIrqLatch;
        } else {
            scanLineCounter--;
        }
        
        if (scanLineCounter == 0 && regIrqEnable) {
            cpu->setInterrupt(cpu::IRQ);
            printf("IRQ: %d\r\n", address);
        }
    }
}

void MapperMMC3::init() {
    Mapper::init();
    
    mirrorType = (MirroringType)0;
    
    prgBankSize = cart.getHeader().prgSize * 2;
    chrBankSize = chrBanks.size();
    
    memset(bankValues, 0, sizeof(bankValues));
    
    mmc3prgBank[0] = prgBanks[0];
    mmc3prgBank[1] = prgBanks[1];
    mmc3prgBank[2] = prgBanks[prgBanks.size() - 2];
    mmc3prgBank[3] = prgBanks[prgBanks.size() - 1];
    
    onSelectChr();
    onSelectPrg();
    regIrqEnable = 1;
}

void MapperMMC3::writePRG(Address addr, Byte value) {
    if (addr < 0x8000) {
        if (!(regRamProtect & 0x40)) {
            prgRam[addr - 0x6000] = value;
        }
        
        return;
    }
    
    switch(addr & 0xE001) {
        case 0x8000: {
            bankSelectAddress = addr;
            regBankSelect = value;
            
            onSelectPrg();
            onSelectChr();
        } break;
            
        case 0x8001: {
            regBankData = value;
            
            if ((regBankSelect & 0x7) <= 1) {
                regBankData = value & 0xFE;
            }
            
            bankValues[regBankSelect & 7] = regBankData;
            
            onSelectPrg();
            onSelectChr();
        } break;
            
        case 0xA000: {
            mirrorType = (value & 1) ? Horizontal : Vertical;
            mirroringCallback(cbVal);
        } break;
            
        case 0xA001: {
            regRamProtect = value;
        } break;
            
        case 0xC000: {
            regIrqLatch = value;
        } break;
            
        case 0xC001: {
            regIrqReload = 1;
            //scanLineCounter = 0;
        } break;
            
        case 0xE000: {
            regIrqDisable = 1;
            regIrqEnable = 0;
        } break;
            
        case 0xE001: {
            regIrqEnable = 1;
        } break;
    }
}

void MapperMMC3::updateA12(bool next) {
  
}

Byte MapperMMC3::readCHR(Address addr) {
    onScanlineCount(addr);
    
    if (addr < 0x2000) {
        return mmc3chrBank[addr / 0x400][addr % 0x400];
    }
    
    return 0;
}

void MapperMMC3::writeCHR(Address addr, Byte value) {
    if (addr < 0x2000) {
        mmc3chrBank[addr / 0x400][addr % 0x400] = value;
    }
}

Byte MapperMMC3::readPRG(Address addr) {
    
    if (addr < 0x6000) {
        return 0;
    } else if (addr < 0x8000) {
        return regRamProtect & 0x80 ? prgRam[addr - 0x6000] : 0xFF;
    }
    
    return mmc3prgBank[(addr - 0x8000) / 0x2000][addr % 0x2000];
}
