//
//  Cartridge.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Cartridge_hpp
#define Cartridge_hpp

#include "common.h"

namespace nes::mapper {
    class Mapper;
}

namespace nes::system {
    class StateManager;
}

namespace nes::common {
    
    struct NesHeader {
        char type[4];
        Byte prgSize;
        Byte chrSize;
        
        union {
            Byte flags6;
            struct {
                char mt : 1;
                char b : 1;
                char tr : 1;
                char m4 : 1;
                char mn : 4;
            } Bit;
        } mainFlags;
        
        Byte flags7;
        Byte flags8;
        Byte flags9;
        Byte flags10;
        Byte padding;
        uint32_t morePadding;
    };
    
    class Cartridge {
    public:
        Cartridge(system::StateManager &stateManager) : stateManager(stateManager) {};
        ~Cartridge() {};
        
        bool load(const string &fileName);
        
        const NesHeader &getHeader() { return header; }
        
        nes::mapper::Mapper &getMapper() { return *mapper; }
        
        Byte *getChrData() { return chrData; }
        Byte *getPrgData() { return prgData; }
        
        const string &getFileName() { return fileName; }
        
        void loadBattery();
        void saveBattery();
        
    private:
        system::StateManager &stateManager;
        
        NesHeader header;
        string fileName;
        
        nes::mapper::Mapper *mapper;
        
        Byte *chrData = nullptr;
        Byte *prgData = nullptr;
    };
}

#endif /* Cartridge_hpp */
