//
//  PpuBus.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef PpuBus_hpp
#define PpuBus_hpp

#include "common.h"

#include "Mapper.hpp"

using namespace nes::mapper;

namespace nes::ppu {
    
    class PpuBus {
    public:
        PpuBus();
        ~PpuBus() {}
        Byte read(Address address);
        void write(Address addr, Byte value);
        void reset();
        void updateMirroring();
        
        void setMapper(Mapper *p) { mapper = p; }
        
        Byte readPalette(Byte addr) { return palette[addr]; }
        
        bool getA12Status() { return a12Status; }
        
        Mapper *getMapper() { return mapper; }
        
        bool loadState(std::ifstream &in) ;
        bool saveState(std::ofstream &out) ;
    private:
        
        Byte *busRam = nullptr;
        Byte *palette = nullptr;
        Address nameTables[4];
        Mapper *mapper;
        bool a12Status = false;
    };
    
}

#endif /* PpuBus_hpp */
