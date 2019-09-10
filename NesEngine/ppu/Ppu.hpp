//
//  Ppu.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Ppu_hpp
#define Ppu_hpp

#include "common.h"
#include "PpuBus.hpp"
#include "PipelineProcessor.hpp"

#define ScanlineCycleLength 341
#define ScanlineEndCycle 340
#define VisibleScanlines 240
#define ScanlineVisibleDots 256
#define FrameEndScanline 261

namespace nes::ppu {
    
    enum ControlFlags {
        Nametable1 = 1 << 0,
        Nametable2 = 1 << 1,
        IncMode = 1 << 2,
        SpriteTileSelect = 1 << 3,
        BackTileSelect = 1 << 4,
        HeightSprites = 1 << 5,
        PPuMS = 1 << 6,
        NMIEnable = 1 << 7
    };
    
    enum MaskFlags {
        GreyScale = 1 << 0,
        BGLeftEnabled = 1 << 1,
        SpriteLeftEnabled = 1 << 2,
        BGEnabled = 1 << 3,
        SpriteEnabled = 1 << 4,
        ERed = 1 << 5,
        EGreen = 1 << 6,
        EBlue = 1 << 7
    };
    
    enum StatusFlags {
        Overflow = 1 << 5,
        SpriteZero = 1 << 6,
        VBlank = 1 << 7
    };
    
    enum PpuState {
        PPU_STATE_PRE,
        PPU_STATE_RENDER,
        PPU_STATE_VBLANK,
        PPU_STATE_POST
    };
    
    struct PpuData {
        Byte regStatus;
        Byte regControl;
        Byte regMask;
        PpuState pipelineState;
        Word addIncrement;
        Byte dataBuffer;
        Byte spriteDataAddress;
        
        bool evenFrame;
        bool firstWrite;
        
        Word dataAddress;
        Word tempAddress;
        Word cycle;
        Word scanLine;
        Word fineXScroll;
        
        Byte scanLineSprites[64];
        Byte scanLineSpritesSize;
        Byte spriteMemory[64 * 4];
        
        uint32_t **pictureBuffer;
        uint32_t curFrame;
    };
    
    class Ppu {
    public:
        Ppu();
        ~Ppu() {}
        
        void clockTick();
        
        Byte readStatus();
        Byte readData();
        void reset();
        
        void setScroll(Byte scroll);
        
        Byte readControl() { return ppuData.regControl; }
        Byte readMask() { return ppuData.regMask; }
        
        void doDma(Byte *p);
        
        void setStatus(Byte b) {
            ppuData.regStatus = b;
        }
        
        void setControl(Byte b) {
            ppuData.regControl = b;
            
            if (b & 0x04) {
                ppuData.addIncrement = 0x20;
            } else {
                ppuData.addIncrement = 1;
            }
            
            ppuData.tempAddress &= ~0xC00;
            ppuData.tempAddress |= (b & 0x3) << 10;
        }
        
        void setMask(Byte b) {
            ppuData.regMask = b;
        }
        
        Byte readOam(Address addr) { return ppuData.spriteMemory[addr]; }
        
        Byte readOam() { return readOam(ppuData.spriteDataAddress); }
        
        void writeOamAddress(Address addr, Byte value) {
            ppuData.spriteMemory[addr] = value;
        }
        
        void writeOam(Byte value) {
            writeOamAddress(ppuData.spriteDataAddress++, value);
        }
        
        void setData(Byte data) {
            ppuBus.write(ppuData.dataAddress, data);
            ppuData.dataAddress += ppuData.addIncrement;
        }
        
        void setDataAddress(Address addr);
        
        void setOamAddress(Address addr) { ppuData.spriteDataAddress = addr; }
        
        void setControlFlag(ControlFlags flag, bool value) {
            if (value) {
                ppuData.regControl |= (Byte)flag;
            } else {
                ppuData.regControl &= ~(Byte)flag;
            }
        }
        
        void setMaskFlag(MaskFlags flag, bool value) {
            if (value) {
                ppuData.regMask |= (Byte)flag;
            } else {
                ppuData.regMask &= ~(Byte)flag;
            }
        }
        
        void setStatusFlag(StatusFlags flag, bool value) {
            Byte b = (Byte)flag;
            
            if (value) {
                ppuData.regStatus |= b;
            } else {
                ppuData.regStatus &= ~b;
            }
            
            return;
        }
        
        bool getMaskFlag(MaskFlags flag) {
            return (ppuData.regMask & (Byte)flag) != 0;
        }
        
        bool getStatusFlag(StatusFlags flag) {
            return (ppuData.regStatus & (Byte)flag) != 0;
        }
        
        bool getControlFlag(ControlFlags flag) {
            return (ppuData.regControl & (Byte)flag) != 0;
        }
        
        PpuBus &bus() {
            return ppuBus;
        }
        
        PpuData &data() {
            return ppuData;
        }
        
        PipelineProcessor &processor() {
            return pipelineProcessor;
        }
        
    private:
        PpuData ppuData;
        PpuBus ppuBus;
        PipelineProcessor pipelineProcessor;
    };
    
}

#endif /* Ppu_hpp */
