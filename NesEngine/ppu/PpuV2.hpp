//
//  PpuV2.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/18/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef PpuV2_hpp
#define PpuV2_hpp

#include "common.h"
#include "PpuBus.hpp"

namespace nes::ppu {
    enum PpuRegisters {
        PpuControl = 0x2000,
        PpuMask,
        PpuStatus,
        OAMAddress,
        OAMData,
        PpuScroll,
        PpuAddress,
        PpuData,
        OAMDMA = 0x4014,
    };
    
    union ControlRegister {
        Byte reg;
        struct {
            Byte nameTable: 2;
            Byte incMode : 1;
            Byte spriteTileSelect : 1;
            Byte backTileSelect : 1;
            Byte tallSprites : 1;
            Byte masterSlave : 1;
            Byte nmi : 1;
        } flag;
    };
    
    union MaskRegister {
        Byte reg;
        struct {
            Byte greyScale: 1;
            Byte BGLeft : 1;
            Byte SpriteLeft : 1;
            Byte BGEnabled : 1;
            Byte SpriteEnabled : 1;
            Byte ERed : 1;
            Byte EGreen : 1;
            Byte EBlue : 1;
        } flag;
    };
    
    union StatusRegister {
        Byte reg;
        struct {
            Byte na: 5;
            Byte overflow : 1;
            Byte spriteZero : 1;
            Byte vBlank : 1;
        } flag;
    };
    
    struct PpuState {
        ControlRegister controlRegister;
        MaskRegister maskRegister;
        StatusRegister statusRegister;
        
        int scanLine;
        int cycle;
        bool nmiSuppressed;
        bool writeLatch;
        Byte *oamData;
        Address oamAddress;
        Address vramAddress;
        Byte readBuffer;
    };
    
    class PpuV2 {
    public:
        void reset();
        
    private:
    
        Byte readRegister(PpuRegisters reg);
        void writeRegister(PpuRegisters reg, Byte b);
        
        PpuState state;
    };
}

#endif /* PpuV2_hpp */
