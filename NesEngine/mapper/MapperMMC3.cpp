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
        mmc3chrBank[4] = chrBanks[((bankValues[0] ) % chrBankSize)];
        mmc3chrBank[5] = chrBanks[((bankValues[0] + 1) % chrBankSize)];
        mmc3chrBank[6] = chrBanks[((bankValues[1] )  %chrBankSize)];
        mmc3chrBank[7] = chrBanks[((bankValues[1] + 1)  %chrBankSize)];
    } else {
        //chr banks
        mmc3chrBank[0] = chrBanks[((bankValues[0] ) % chrBankSize)];
        mmc3chrBank[1] = chrBanks[((bankValues[0] + 1) %chrBankSize)];
        mmc3chrBank[2] = chrBanks[((bankValues[1] )  %chrBankSize)];
        mmc3chrBank[3] = chrBanks[((bankValues[1] +1) % chrBankSize)];
        mmc3chrBank[4] = chrBanks[((bankValues[2]) % chrBankSize)];
        mmc3chrBank[5] = chrBanks[((bankValues[3]) % chrBankSize)];
        mmc3chrBank[6] = chrBanks[((bankValues[4]) % chrBankSize)];
        mmc3chrBank[7] = chrBanks[((bankValues[5])  %chrBankSize)];
    }
}


void MapperMMC3::onSelectPrg() {
    
    if (regBankSelect & 0x40) {
        //prg banks
        mmc3prgBank[0] = prgBanks[prgBanks.size() - 2];
        mmc3prgBank[1] = prgBanks[bankValues[7]];
        mmc3prgBank[2] = prgBanks[bankValues[6]];
        mmc3prgBank[3] = prgBanks[(prgBankSize - 1)];
    } else {
        //prg banks
        mmc3prgBank[0] = prgBanks[(bankValues[6] % prgBankSize)];
        mmc3prgBank[1] = prgBanks[(bankValues[7] % prgBankSize)];
        mmc3prgBank[2] = prgBanks[(prgBankSize - 2)];
        mmc3prgBank[3] = prgBanks[(prgBankSize - 1)];
    }
}

void MapperMMC3::init() {
    Mapper::init();
    
    mirrorType = (MirroringType)0;
    
    prgBankSize = cart.getHeader().prgSize * 2;
    chrBankSize = chrBanks.size();
    
    memset(bankValues, 0, sizeof(bankValues));
    onSelectChr();
    
    mmc3prgBank[2] = prgBanks[prgBanks.size() - 2];
    mmc3prgBank[3] = prgBanks[prgBanks.size() - 1];
    
    regIrqEnable = 1;
}

void MapperMMC3::writePRG(Address addr, Byte value) {
    //printf("WRITE PRG: %0.4X = %0.2X\r\n", addr, value);
    
    if (addr < 0x8000) {
        if (regRamProtect & 0x40) {
            prgRam[addr - 0x6000] = value;
        }
    } else if (addr < 0xA000 && (addr % 2) == 0) {
        bankSelectAddress = addr;
        regBankSelect = value;
        
        onSelectPrg();
        onSelectChr();
    } else if (addr < 0xA000 && (addr % 2) == 1) {
        regBankData = value;
        
        if ((regBankSelect & 0x7) <= 1) {
            regBankData = value & 0xFE;
        }
        
        Byte bank = regBankSelect & 7;
        bankValues[bank] = regBankData;
        
        onSelectPrg();
        onSelectChr();
        
    } else if (addr < 0xC000 && (addr % 2) == 0) {
        regMirroring = value;
        mirrorType = (MirroringType)(Byte)(value & 1);
        mirroringCallback(cbVal);
        
        //printf("MTX: %d\r\n", mirrorType);
        
    } else if (addr < 0xC000 && (addr % 2) == 1) {
        regRamProtect = value;
        
    } else if (addr < 0xE000 && (addr % 2) == 0) {
        regIrqLatch = value;
        //printf("Set IRQ Latch: %0.2X\r\n", value);
        /*
        if (ppu->data().scanLine < 240) {
            regIrqReload |= 0x80;
            irqPreset = 0xFF;
        } else {
            regIrqReload |= 0x80;
            irqPresetVbl = 0xFF;
            irqPreset = 0;
        }*/
        
        
    } else if (addr < 0xE000 && (addr % 2) == 1) {
        regIrqReload = 1;
        scanLineCounter = 0;
        //printf("Set IRQ Reload: %0.2X\r\n", value);
    } else if (addr <= 0xFFFF && (addr % 2) == 0) {
        //printf("Set IRQ Disable: %0.2X\r\n", value);
        regIrqDisable = 1;
        regIrqEnable = 0;
    } else if (addr <= 0xFFFF && (addr % 2) == 1) {
        
        //printf("Set IRQ Enable: %0.2X\r\n", value);
        regIrqEnable = 1;
    }
}

void MapperMMC3::updateA12(bool next) {
    if (lastA12 == false && next) {
        //printf("A12 HIGH: %d - %d - %d\r\n", scanLineCounter, regIrqReload, regIrqEnable);
        LOG << "A12 WENT HIGH: " << scanLineCounter << " - " << regIrqReload << " - "  << regIrqEnable << " - " << ppu->displayEnabled() << endl;
        
        if (regIrqReload || scanLineCounter == 0) {
            scanLineCounter = regIrqLatch;
            regIrqReload = 0;
        } else {
            scanLineCounter--;
        }
        
        if (scanLineCounter == 0 && regIrqEnable ) { //&& ppu->displayEnabled()
            cpu->setInterrupt(cpu::IRQ);
            LOG << "SET INTERRUPT" << endl;
            //printf("SET IRQ\r\n");
        }
        
        /*
        if (regIrqReload == 0) {
            regIrqReload = regIrqLatch;
            printf("SET TO LATCH: %d\r\n", regIrqReload);
        } else {
            printf("DECREMENT\r\n");
            regIrqReload--;
        }
        
        if (regIrqReload == 0) {
            if ( regIrqEnable) {
                //trigger
                cpu->setInterrupt(cpu::IRQ);
                printf("SET IRQ\r\n");
            }
        }*/
    }
    
    lastA12 = next;
}

Byte MapperMMC3::readCHR(Address addr) {
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
        return regRamProtect & 0x80 ? prgRam[addr - 0x6000] : 0;
    }
    
    return mmc3prgBank[(addr - 0x8000) / 0x2000][addr % 0x2000];
}
