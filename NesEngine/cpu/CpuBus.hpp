//
//  CpuBus.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef CpuBus_hpp
#define CpuBus_hpp

#include "common.h"
#include "Mapper.hpp"
#include "Ppu.hpp"
#include "Apu.hpp"

using namespace nes::mapper;
using namespace nes::ppu;

namespace nes::cpu {
    class Cpu;
    
    enum IORegisters
    {
        PPUCTRL = 0x2000,
        PPUMASK,
        PPUSTATUS,
        OAMADDR,
        OAMDATA,
        PPUSCROL,
        PPUADDR,
        PPUDATA,
        OAMDMA = 0x4014,
        JOY1 = 0x4016,
        JOY2 = 0x4017,
    };
    
    class CpuBus {
    public:
        CpuBus(){}
        ~CpuBus(){}
        
        Byte read(Address addr);
        void write(Address addr, Byte b);
        
        Word readWord(Address addr);
        Word readWord(Address addr, Address offset);
        void writeWord(Address addr, Word b);
        
        Byte *page(Byte page);
        
        void reset();
        
        void setMapper(Mapper *p) {mapper = p;}
        
        void setPpu(Ppu *p) {ppu = p;}
        
        void setCpu(Cpu *p) {cpu = p; apu.setCpu(p);}
        
        Mapper *getMapper() {return mapper; }
        
        apu::Apu *getApu() { return &apu; }
        
    private:
        Byte prgRam[64 * 1024];
        Byte ram[0x800];
        Mapper *mapper;
        Ppu *ppu;
        Cpu *cpu;
        apu::Apu apu;
    };
    
}

#endif /* CpuBus_hpp */
