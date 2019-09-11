//
//  MapperMMC3.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/9/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef MapperMMC3_hpp
#define MapperMMC3_hpp

#include "Mapper.hpp"

namespace nes::mapper {
    class MapperMMC3: public Mapper {
    public:
        MapperMMC3(Cartridge &cart);
        virtual ~MapperMMC3();
        
        virtual Byte readPRG(Address addr);
        virtual Byte readCHR(Address addr);
        
        virtual void writePRG(Address addr, Byte value);
        virtual void writeCHR(Address addr, Byte value);
        
        virtual bool loadState(std::ifstream &in) { return true; }
        virtual bool saveState(std::ofstream &out) { return true; }
        
        virtual int getPrgBankSize() { return 0x2000; };
        virtual int getChrBankSize() { return 0x0400; };
        
        virtual void init();
        virtual void updateA12(bool status);
        
    private:
        void onSelectChr();
        void onSelectPrg();
        
        Byte bankValues[8];
        
        uint16_t bankSelectAddress;
        
        Byte regBankSelect = 0;
        Byte regBankData = 0;
        Byte regMirroring = 0;
        Byte regRamProtect = 0x80;
        Byte regIrqLatch = 0;
        Byte regIrqReload = 0;
        Byte regIrqDisable = 0;
        Byte regIrqEnable = 0;
        
        Word prgBankSize;
        Word chrBankSize;
        
        Byte *mmc3prgBank[4];
        Byte *mmc3chrBank[8];
        
        int irqPresetVbl = 0;
        int irqPreset = 0;
        bool lastA12 = false;
        
        Word scanLineCounter = 0;
    };
}

#endif /* MapperMMC3_hpp */
