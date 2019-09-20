//
//  Ppu.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Ppu.hpp"
#include "Log.hpp"

using namespace nes::ppu;

Ppu::Ppu() {
    pipelineProcessor.setPpu(this);
}

void Ppu::reset() {
    ppuData.regStatus = 0;
    ppuData.regControl = 0;
    ppuData.regMask = 0;
    ppuData.pipelineState = PPU_STATE_PRE;
    ppuData.cycle = 0;
    ppuData.evenFrame = 1;
    ppuData.tempAddress = 0;
    ppuData.dataAddress = 0;
    ppuData.scanLine = 0;
    ppuData.firstWrite = true;
    ppuData.addIncrement = 1;
    ppuData.dataBuffer = 0;
    ppuData.fineXScroll = 0;
    ppuData.scanLineSpritesSize = 0;
    memset(ppuData.spriteMemory, 0, 64 * 4);
    ppuData.curFrame = 0;
    ppuData.scanLine = 261;
    
    ppuData.pictureBuffer = new uint32_t *[ScanlineVisibleDots];
    ppuData.pixelBuffer = new PixelData *[ScanlineVisibleDots];
    ppuData.colorData = new Byte *[ScanlineVisibleDots];
    
    for (int i=0; i<ScanlineVisibleDots; i++) {
        ppuData.pixelBuffer[i] = new PixelData[VisibleScanlines];
        memset(ppuData.pixelBuffer[i], 0, sizeof(PixelData) * VisibleScanlines);
        
        ppuData.pictureBuffer[i] = new uint32_t[VisibleScanlines];
        memset(ppuData.pictureBuffer[i], 0, sizeof(uint32_t) * VisibleScanlines);
        
        ppuData.colorData[i] = new Byte[VisibleScanlines];
        memset(ppuData.colorData[i], 0, sizeof(Byte) * VisibleScanlines);
    }
    
    ppuBus.reset();
    
    ppuData.spriteDataAddress = 0;
    
    setMaskFlag(SpriteEnabled, true);
    setMaskFlag(BGEnabled, true);
    setMaskFlag(BGLeftEnabled, true);
    setMaskFlag(SpriteLeftEnabled, true);
    
    setStatusFlag(Overflow, 1);
    setStatusFlag(VBlank, 1);
    
    bus().updateMirroring();
}

bool Ppu::saveState(std::ofstream &stream) {
    stream.write((char *)&ppuData, sizeof(ppuData));
    bus().saveState(stream);
    return true;
}

bool Ppu::loadState(std::ifstream &stream) {
    stream.read((char *)&ppuData, sizeof(ppuData));
    
    ppuData.pictureBuffer = new uint32_t *[ScanlineVisibleDots];
    ppuData.pixelBuffer = new PixelData *[ScanlineVisibleDots];
    ppuData.colorData = new Byte *[ScanlineVisibleDots];
    
    for (int i=0; i<ScanlineVisibleDots; i++) {
        ppuData.pixelBuffer[i] = new PixelData[VisibleScanlines];
        memset(ppuData.pixelBuffer[i], 0, sizeof(PixelData) * VisibleScanlines);
        
        ppuData.pictureBuffer[i] = new uint32_t[VisibleScanlines];
        memset(ppuData.pictureBuffer[i], 0, sizeof(uint32_t) * VisibleScanlines);
        
        ppuData.colorData[i] = new Byte[VisibleScanlines];
        memset(ppuData.colorData[i], 0, sizeof(Byte) * VisibleScanlines);
    }
    bus().loadState(stream);
    
    return true;
}

void Ppu::clockTick() {
    switch(ppuData.pipelineState) {
        case PPU_STATE_PRE:
            pipelineProcessor.preRender();
            break;
        case PPU_STATE_RENDER:
            pipelineProcessor.render();
            break;
        case PPU_STATE_POST:
            pipelineProcessor.postRender();
            break;
        case PPU_STATE_VBLANK:
            pipelineProcessor.vBlank();
            break;
    }
    
    ppuData.cycle++;
}

Byte Ppu::readStatus() {
    Byte temp = ppuData.regStatus & 0xE0;
    temp |= ppuData.regControl & 0x1F;
    
    ppuData.firstWrite = true;
    setStatusFlag(VBlank, false);
    
    if (ppuData.scanLine == 241 && ppuData.cycle == 0) {
        temp &= ~0x80;
    }
    
    return temp;
}

Byte Ppu::readData() {
    Byte data = ppuBus.read(ppuData.dataAddress);
    
    ppuData.dataAddress += ppuData.addIncrement;
    
    if (ppuData.dataAddress < 0x3F00) {
        std::swap(ppuData.dataBuffer, data);
    }
    
    return data;
}



void Ppu::setDataAddress(Address addr) {
    if (ppuData.firstWrite) {
        ppuData.tempAddress &= ~0xff00;
        ppuData.tempAddress |= (addr & 0x3F) << 8;
        ppuData.firstWrite = false;
    } else {
        ppuData.tempAddress &= ~0xFF;
        ppuData.tempAddress |= addr;
        ppuData.dataAddress = ppuData.tempAddress;
        ppuData.firstWrite = true;
        
        //printf("DATA ADDR SET TO %0.4X\r\n", ppuData.dataAddress);
        ppuBus.getMapper()->updateA12(((ppuData.dataAddress >> 12) & 1));
    }
}

void Ppu::doDma(Byte *page) {
    
    memcpy(ppuData.spriteMemory + ppuData.spriteDataAddress, page, 256 - ppuData.spriteDataAddress);
    
    if (ppuData.spriteDataAddress) {
        memcpy(ppuData.spriteMemory, page + (256 - ppuData.spriteDataAddress), ppuData.spriteDataAddress);
    }
}

void Ppu::setScroll(Byte scroll) {
    if (ppuData.firstWrite)
    {
        ppuData.tempAddress &= ~0x1f;
        ppuData.tempAddress |= (scroll >> 3) & 0x1f;
        ppuData.fineXScroll = scroll & 0x7;
        ppuData.firstWrite = false;
    }
    else
    {
        ppuData.tempAddress &= ~0x73e0;
        ppuData.tempAddress |= ((scroll & 0x7) << 12) |
        ((scroll & 0xf8) << 2);
        ppuData.firstWrite = true;
    }
}
