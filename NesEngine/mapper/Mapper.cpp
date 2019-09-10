//
//  Mapper.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Mapper.hpp"
#include "Log.hpp"

#include "MapperNRom.hpp"
#include "MapperMMC1.hpp"

using namespace nes::mapper;

Mapper::Mapper(Cartridge &cart) : cart(cart) {
    
}

Mapper::~Mapper() {
    if (chrRam) {
        delete [] chrRam;
    }
}

void Mapper::init() {
    int numPrgBanks = cart.getHeader().prgSize * (0x4000 / getPrgBankSize());
    int numChrBanks = cart.getHeader().chrSize * (0x2000 / getChrBankSize());
    
    mirrorType = (MirroringType)(cart.getHeader().mainFlags.flags6 & 1);
    
    for (int i=0; i<numPrgBanks; i++) {
        prgBanks.push_back(cart.getPrgData() + (getPrgBankSize() * i));
        Byte *p = prgBanks[i];
        
        LOG << "PRG Banks: " << i << " at " << hex64 << (uint64_t)p << endl;
        
    }
    
    for (int i=0; i<numChrBanks; i++) {
        chrBanks.push_back(cart.getChrData() + (getChrBankSize() * i));
        Byte *p = chrBanks[i];
        
        LOG << "CHR Banks: " << i << " at " << hex64 << (uint64_t)p << endl;
    }
    
    if (!numChrBanks) {
        chrRam = new Byte[0x2000];
    } else {
        chrRam = nullptr;
    }
}

Mapper *MapperFactory::CreateMapper(Cartridge &cart) {
    Mapper *mapper = nullptr;
    
    switch(cart.getHeader().mainFlags.Bit.mn) {
        case 0:
            mapper = new MapperNRom(cart);
            break;
        case 1:
            mapper = new MapperMMC1(cart);
            break;
            
        default:
            return nullptr;
    }
    
    mapper->init();
    
    return mapper;
}
