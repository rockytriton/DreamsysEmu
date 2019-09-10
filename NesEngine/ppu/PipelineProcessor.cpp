//
//  PipelineProcessor.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "PipelineProcessor.hpp"
#include "Ppu.hpp"
#include "Cpu.hpp"
#include "Log.hpp"

using namespace nes::ppu;

const uint32_t colors[] = {
    0x666666ff, 0x002a88ff, 0x1412a7ff, 0x3b00a4ff, 0x5c007eff, 0x6e0040ff, 0x6c0600ff, 0x561d00ff,
    0x333500ff, 0x0b4800ff, 0x005200ff, 0x004f08ff, 0x00404dff, 0x000000ff, 0x000000ff, 0x000000ff,
    0xadadadff, 0x155fd9ff, 0x4240ffff, 0x7527feff, 0xa01accff, 0xb71e7bff, 0xb53120ff, 0x994e00ff,
    0x6b6d00ff, 0x388700ff, 0x0c9300ff, 0x008f32ff, 0x007c8dff, 0x000000ff, 0x000000ff, 0x000000ff,
    0xfffeffff, 0x64b0ffff, 0x9290ffff, 0xc676ffff, 0xf36affff, 0xfe6eccff, 0xfe8170ff, 0xea9e22ff,
    0xbcbe00ff, 0x88d800ff, 0x5ce430ff, 0x45e082ff, 0x48cddeff, 0x4f4f4fff, 0x000000ff, 0x000000ff,
    0xfffeffff, 0xc0dfffff, 0xd3d2ffff, 0xe8c8ffff, 0xfbc2ffff, 0xfec4eaff, 0xfeccc5ff, 0xf7d8a5ff,
    0xe4e594ff, 0xcfef96ff, 0xbdf4abff, 0xb3f3ccff, 0xb5ebf2ff, 0xb8b8b8ff, 0x000000ff, 0x000000ff,
};

void PipelineProcessor::preRender() {
    PpuData &ppuData = ppu->data();
    
    if (ppuData.cycle == 1) {
        ppu->setStatusFlag(SpriteZero, false);
        
    } else if (ppuData.cycle == ScanlineVisibleDots + 2 && ppu->getMaskFlag(BGEnabled) && ppu->getMaskFlag(SpriteEnabled)) {
        ppuData.dataAddress &= ~0x41F;
        ppuData.dataAddress |= ppuData.tempAddress & 0x41F;
        
    } else if (ppuData.cycle > 280 && ppuData.cycle <= 304 && ppu->getMaskFlag(BGEnabled) && ppu->getMaskFlag(SpriteEnabled)) {
        ppuData.dataAddress &= ~0x7BE0;
        ppuData.dataAddress |= ppuData.tempAddress & 0x7BE0;
    }
    
    if (ppuData.cycle >= ScanlineEndCycle - (!ppuData.evenFrame && ppu->getMaskFlag(BGEnabled) && ppu->getMaskFlag(SpriteEnabled))) {
        ppuData.pipelineState = PPU_STATE_RENDER;
        ppuData.cycle = 0;
        ppuData.scanLine = 0;
        ppu->setStatusFlag(VBlank, false);
    }
}

void PipelineProcessor::renderVisibleDots() {
    PpuData &ppuData = ppu->data();
    
    Byte bgColor = 0;
    Byte sprColor = 0;
    bool bgOpaque = false;
    bool sprOpaque = true;
    bool spriteForeground = false;
    
    Word x = ppuData.cycle - 1;
    Word y = ppuData.scanLine;
    
    if (ppu->getMaskFlag(BGEnabled)) {
        Word xFine = (ppuData.fineXScroll + x) % 8;
        
        if (ppu->getMaskFlag(BGLeftEnabled) || x >= 8) {
            Word addr = 0x2000 | (ppuData.dataAddress & 0x0FFF); //tile
            Byte tile = ppu->bus().read(addr);
            addr = (tile * 16) + ((ppuData.dataAddress >> 12) & 0x7);
            addr |= ppu->getControlFlag(BackTileSelect) << 12;
            
            bgColor = (ppu->bus().read(addr) >> (7 ^ xFine)) & 1;
            bgColor |= ((ppu->bus().read(addr + 8) >> (7 ^ xFine)) & 1) << 1;
            
            bgOpaque = bgColor;
            
            addr = 0x23C0 | (ppuData.dataAddress & 0x0C00) | ((ppuData.dataAddress >> 4) & 0x38) | ((ppuData.dataAddress >> 2) & 0x07);
            Byte att = ppu->bus().read(addr);
            int shift = ((ppuData.dataAddress >> 4) & 4) | (ppuData.dataAddress & 2);
            bgColor |= ((att >> shift) & 0x3) << 2;
        }
        
        if (xFine == 7) {
            if ((ppuData.dataAddress & 0x001F) == 31) {
                ppuData.dataAddress &= ~0x001F;
                ppuData.dataAddress ^= 0x0400;
            } else {
                ppuData.dataAddress++;
            }
        }
    }
    
    if (ppu->getMaskFlag(SpriteEnabled) && (ppu->getMaskFlag(SpriteLeftEnabled) || x >= 8)) {
        
        for (int ii=0; ii<ppuData.scanLineSpritesSize; ii++) {
            int i = ppuData.scanLineSprites[ii];
            Byte sx = ppuData.spriteMemory[i * 4 + 3];
            
            if (0 > x - sx || x - sx >= 8) {
                continue;
            }
            
            Byte sy = ppuData.spriteMemory[i * 4 + 0] + 1;
            Byte tile = ppuData.spriteMemory[i * 4 + 1];
            Byte att = ppuData.spriteMemory[i * 4 + 2];
            
            int len = ppu->getControlFlag(HeightSprites) ? 16 : 8;
            int xShift = (x - sx) % 8;
            int yOffset = (y - sy) % len;
            
            if ((att & 0x40) == 0) {
                xShift ^= 7;
            }
            
            if ((att & 0x80) != 0) {
                yOffset ^= (len - 1);
            }
            
            Word addr = 0;
            
            if (!(ppu->getControlFlag(HeightSprites))) {
                addr = tile * 16 + yOffset;
                
                if (ppu->getControlFlag(SpriteTileSelect)) {
                    addr += 0x1000;
                }
            } else {
                yOffset = (yOffset & 7) | ((yOffset & 8) << 1);
                addr = (tile >> 1) * 32 + yOffset;
                addr |= (tile & 1) << 12;
            }
            
            sprColor |= (ppu->bus().read(addr) >> (xShift)) & 1;
            sprColor |= ((ppu->bus().read(addr + 8) >> (xShift)) & 1) << 1;
            
            if (!(sprOpaque = sprColor)) {
                sprColor = 0;
                continue;
            }
            
            sprColor |= 0x10;
            sprColor |= (att & 0x3) << 2;
            
            spriteForeground = !(att & 0x20);
            
            if (!(ppu->getStatusFlag(SpriteZero)) && ppu->getMaskFlag(BGEnabled) && i == 0 && sprOpaque && bgOpaque) {
                ppu->setStatusFlag(SpriteZero, 1);
            }
            
            break;
        }
    }
    
    Byte paletteAddr = bgColor;
    
    if ((!bgOpaque && sprOpaque) || (bgOpaque && sprOpaque && spriteForeground)) {
        paletteAddr = sprColor;
    } else if (!bgOpaque && !sprOpaque) {
        paletteAddr = 0;
    }
    
    Byte color = ppu->bus().readPalette(paletteAddr);
    
    ppuData.pictureBuffer[x][y] = colors[color];
}

void PipelineProcessor::render() {
    PpuData &ppuData = ppu->data();
    
    if (ppuData.cycle > 0 && ppuData.cycle <= ScanlineVisibleDots) {
        renderVisibleDots();
    } else if (ppuData.cycle == ScanlineVisibleDots + 1 && ppu->getMaskFlag(BGEnabled)) {
        if ((ppuData.dataAddress & 0x7000) != 0x7000) {
            ppuData.dataAddress += 0x1000;
        } else {
            ppuData.dataAddress &= ~0x7000;
            uint16_t y = (ppuData.dataAddress & 0x03E0) >> 5;
            if (y == 29) {
                y = 0;
                ppuData.dataAddress ^= 0x0800;
            } else if (y == 31) {
                y = 0;
            } else {
                y++;
            }
            
            ppuData.dataAddress = (ppuData.dataAddress & ~0x03E0) | (y << 5);
        }
    } else if (ppuData.cycle == ScanlineVisibleDots + 2 && ppu->getMaskFlag(BGEnabled) && ppu->getMaskFlag(SpriteEnabled)) {
        ppuData.dataAddress &= ~0x41F;
        ppuData.dataAddress |= ppuData.tempAddress & 0x41F;
    }
    
    if (ppuData.cycle >= ScanlineEndCycle) {
        ppuData.scanLineSpritesSize = 0;
        memset(ppuData.scanLineSprites, 0, 64);
        
        int range = 8;
        
        if (ppu->getControlFlag(HeightSprites)) {
            range = 16;
        }
        
        int j = 0;
        for (uint16_t i = ppuData.spriteDataAddress / 4; i < 64; i++) {
            int diff = (ppuData.scanLine - ppuData.spriteMemory[i * 4]);
            
            if (0 <= diff && diff < range) {
                ppuData.scanLineSprites[ppuData.scanLineSpritesSize++] = i;
                j++;
                
                if (j >= 8) {
                    break;
                }
            }
        }
        
        //TODO: if (mapper_get_current()->scanLineCallback) mapper_get_current()->scanLineCallback(ppuData);
        
        ppuData.scanLine++;
        ppuData.cycle = 0;
    }
    
    if (ppuData.scanLine >= VisibleScanlines) {
        ppuData.pipelineState = PPU_STATE_POST;
    }
}

void PipelineProcessor::postRender() {
    PpuData &ppuData = ppu->data();
    
    if (ppuData.cycle >= ScanlineEndCycle) {
        
        //TODO: if (mapper_get_current()->scanLineCallback) mapper_get_current()->scanLineCallback(ppuData);
        
        ppuData.scanLine++;
        ppuData.cycle = 0;
        ppuData.pipelineState = PPU_STATE_VBLANK;
    }
}

void PipelineProcessor::vBlank() {
    PpuData &ppuData = ppu->data();
    
    if (ppuData.cycle == 1 && ppuData.scanLine == VisibleScanlines + 1) {
        ppu->setStatusFlag(VBlank, true);
        
        if (ppu->getControlFlag(NMIEnable)) {
            cpu->interrupt(cpu::NMI);
        }
    }
    
    if (ppuData.cycle >= ScanlineEndCycle) {
        
        //TODO: if (mapper_get_current()->scanLineCallback) mapper_get_current()->scanLineCallback(ppuData);
        
        ppuData.scanLine++;
        ppuData.cycle = 0;
    }
    
    if (ppuData.scanLine >= FrameEndScanline) {
        ppuData.pipelineState = PPU_STATE_PRE;
        ppuData.scanLine = 0;
        ppuData.evenFrame = !ppuData.evenFrame;
        ppuData.curFrame++;
    }
}

