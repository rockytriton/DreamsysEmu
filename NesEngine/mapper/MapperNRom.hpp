//
//  MapperNRom.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef MapperNRom_h
#define MapperNRom_h

#include "Mapper.hpp"

namespace nes::mapper {
    class MapperNRom : public Mapper {
    public:
        MapperNRom(Cartridge &cart);
        virtual ~MapperNRom();
        
        virtual Byte readPRG(Address addr);
        virtual Byte readCHR(Address addr);
        
        virtual void writePRG(Address addr, Byte value);
        virtual void writeCHR(Address addr, Byte value);
        
        virtual bool loadState(std::ifstream &in) { return true; }
        virtual bool saveState(std::ofstream &out) { return true; }
        
        virtual int getPrgBankSize() { return 0x4000; };
        virtual int getChrBankSize() { return 0x2000; };
        
        virtual void init();
    private:
        bool oneBank;
        Byte *prgRam;
    };
}

#endif /* MapperNRom_h */
