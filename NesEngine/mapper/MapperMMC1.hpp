//
//  MapperMMC1.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/9/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef MapperMMC1_hpp
#define MapperMMC1_hpp

#include "Mapper.hpp"

namespace nes::mapper {
    class MapperMMC1 : public Mapper {
    public:
        MapperMMC1(Cartridge &cart);
        virtual ~MapperMMC1();
        
        virtual Byte readPRG(Address addr);
        virtual Byte readCHR(Address addr);
        
        virtual void writePRG(Address addr, Byte value);
        virtual void writeCHR(Address addr, Byte value);
        
        virtual bool loadState(std::ifstream &in) { return true; }
        virtual bool saveState(std::ofstream &out) { return true; }
        
        virtual int getPrgBankSize() { return 0x4000; };
        virtual int getChrBankSize() { return 0x1000; };
        
        virtual void init();
        
        virtual void saveBattery();
        virtual void loadBattery();
        
    private:
        bool shiftWrite(Address addr, Byte value);
        void switchCharBanks();
        void switchPrgBanks();
        
        Byte regLoad = 0x10;
        Byte regControl = 0;
        Byte regChrBank0 = 0;
        Byte regChrBank1 = 0;
        Byte regPrgBank = 0;
        Word prgSize = 0;
        
        Byte *prgRam;
        Byte *mmc1ChrBanks[2];
        Byte *mmc1PrgBanks[2];
        bool prgRamDisabled = false;
        
        bool checkBatterySave = false;
        hrClock::time_point lastSaveTime = std::chrono::high_resolution_clock::now();
    };
}


#endif /* MapperMMC1_hpp */
