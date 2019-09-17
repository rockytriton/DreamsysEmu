//
//  Mapper.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Mapper_hpp
#define Mapper_hpp

#include "common.h"
#include "Cartridge.hpp"

#include <fstream>

using namespace nes::common;

typedef void (*MirroringCallback)(void *);

namespace nes::cpu {
    class Cpu;
}

namespace nes::ppu {
    class Ppu;
}

namespace nes::mapper {
    enum MirroringType
    {
        Horizontal  = 0,
        Vertical    = 1,
        FourScreen  = 8,
        OneScreenLower,
        OneScreenHigher,
    };
    
    class Mapper {
    public:
        virtual Byte readPRG(Address addr) = 0;
        virtual Byte readCHR(Address addr) = 0;
        
        virtual void writePRG(Address addr, Byte value) = 0;
        virtual void writeCHR(Address addr, Byte value) = 0;
        
        virtual bool loadState(std::ifstream &in) = 0;
        virtual bool saveState(std::ofstream &out) = 0;
        
        virtual void init();
        
        MirroringType getMirrorType() { return mirrorType; }
        
        virtual int getPrgBankSize() = 0;
        virtual int getChrBankSize() = 0;
        
        void setMirroringCallback(MirroringCallback cb, void *p) { mirroringCallback = cb; cbVal = p; }
        
        void setCpu(nes::cpu::Cpu *p) {cpu = p;}
        void setPpu(nes::ppu::Ppu *p) {ppu = p;}
        
        virtual void updateA12(bool status) {}
        
        virtual void saveBattery() {}
        virtual void loadBattery() {}
        
        Byte *getPrgRam() { return prgRam; }
        
    protected:
        Mapper(Cartridge &cart);
        virtual ~Mapper();
        
        MirroringType mirrorType;
        
        vector<Byte *> prgBanks;
        vector<Byte *> chrBanks;
        
        Cartridge &cart;
        
        MirroringCallback mirroringCallback;
        void *cbVal;
        
        Byte *chrRam = nullptr;
        Byte *prgRam = nullptr;
        
        nes::cpu::Cpu *cpu;
        nes::ppu::Ppu *ppu;
    };
    
    class MapperFactory {
    public:
        static Mapper *CreateMapper(Cartridge &cart);
    };
}

#endif /* Mapper_hpp */
