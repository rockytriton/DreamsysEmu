//
//  CpuBus.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "CpuBus.hpp"
#include "Ppu.hpp"
#include "Cpu.hpp"
#include "Controller.hpp"
#include "Log.hpp"

using namespace nes::ppu;
using namespace nes::cpu;

Byte CpuBus::read(Address address) {
    
    if (address < 0x2000) {
        return ram[address & 0x7FF];
    } else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015) {
        //APU stuff
        return 0;
    }
    else if (address == 0x4014) {
        //DMA
        return 0;
    }
    else if (address == JOY1) {
        //JOY1
        return nes::system::Controller::controller1().read();
    }
    else if (address == JOY2) {
        //JOY2
        return nes::system::Controller::controller2().read();
    }
    else if (address < 0x4000 && address < 0x4000 && (address & 0x2007) == PPUSTATUS) {
        return ppu->readStatus();
    } else if (address < 0x4000 && address < 0x4000 && (address & 0x2007) == PPUDATA) {
        return ppu->readData();
    } else if (address < 0x4000 && address < 0x4000 && (address & 0x2007) == OAMDATA) {
        return ppu->readOam();
    } else if (address < 0x4000 && address < 0x4000 && (address & 0x2007) == PPUCTRL) {
        return ppu->readControl();
    } else if (address < 0x4000 && address < 0x4000 && (address & 0x2007) == PPUMASK) {
        return ppu->readMask();
    } else if (address < 0x4000 && address < 0x4000 && (address & 0x2007) == OAMDMA) {
        return 0; //TODO: ppu_oam_read();
    } else if (address >= 0x4020) {
        return mapper->readPRG(address);
    }
    
    return prgRam[address];
}

void CpuBus::write(Address address, Byte value) {
    if (address < 0x2000) {
        ram[address & 0x7FF] = value;
    }
    else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) {
        //APU stuff
    }
    else if (address == OAMDMA) {
        //DMA
        cpu->skipDmaCycle();
        Byte *pp = page(value);
        ppu->doDma(pp);
        return;
    } else if (address == JOY1) {
        //JOY1
        nes::system::Controller::controller1().write(value);
    } else if (address == JOY2) {
        //JOY2
        nes::system::Controller::controller2().write(value);
    } else if (address >= 0x4020) {
        return mapper->writePRG(address, value);
    } else if (address < 0x4000 && (address & 0x2007) == OAMDATA) {
        return ppu->writeOam(value);
    } else if (address < 0x4000 && (address & 0x2007)  == PPUADDR) {
        return ppu->setDataAddress(value);
    } else if (address < 0x4000 && (address & 0x2007)  == PPUDATA) {
        return ppu->setData(value);
    } else if (address < 0x4000 && (address & 0x2007)  == OAMADDR) {
        return ppu->setOamAddress(value);
    } else if (address < 0x4000 && (address & 0x2007) == PPUCTRL) {
        return ppu->setControl(value);
    } else if (address < 0x4000 && (address & 0x2007) == PPUMASK) {
        return ppu->setMask(value);
    } else if (address < 0x4000 && (address & 0x2007) == PPUSTATUS) {
        return;
    } else if (address < 0x4000 && (address & 0x2007) == PPUSCROL) {
        ppu->setScroll(value);
    } else {
        prgRam[address] = value;
    }
}

Byte *CpuBus::page(Byte p) {
    Address addr = p << 8;
    
    if (addr < 0x2000) {
        return &ram[addr & 0x7FF];
    }
    
    return 0;
}

Word CpuBus::readWord(Address addr, Address offset) {

    Address lo = read((Address)(addr + (Address)offset) & 0x00FF);
    Address hi = read((Address)(addr + (Address)offset + 1) & 0x00FF);
    
    return (hi << 8) | lo;
}

Word CpuBus::readWord(Address addr) {
    
    uint16_t lo = read((addr + 0));
    uint16_t hi = read((addr + 1));
    
    return (hi << 8) | lo;
}

void CpuBus::writeWord(Address addr, Word b) {
    
}

void CpuBus::reset() {
    memset(prgRam, 0, 64 * 1024);
    memset(ram, 0, 0x800);
}
