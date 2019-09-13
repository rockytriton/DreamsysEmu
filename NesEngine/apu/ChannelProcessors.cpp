//
//  ChannelProcessors.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "ChannelProcessors.hpp"
#include "ApuChannel.hpp"

using namespace nes::apu;

void Sweep::reload(Byte value) {
    reset = true;
    enabled = value & 0x80;
    period = value & 0x70;
    shift = value & 0x7;
    negate = value & 0x8;
}

int Sweep::tick() {
    if (reset) {
        val = period + 1;
        reset = false;
        return 0;
    }
    
    if (val) {
        val--;
    } else {
        val = period + 1;
        
        if (enabled) {
            Word change = ch->getTimerPeriod() >> shift;
            
            if (!negate) {
                return change;
            } else if (ch->getChan() == 1) {
                return -(change + 1);
            } else if (ch->getChan() == 2) {
                return -change;
            }
        }
    }
    
    return 0;
}

void Envelope::reload(Byte value) {
    loop = (value >> 5) & 1;
    enabled = value & 0x10;
    //period = value & 0x0F;
    setVolume(value & 0x0F, (value >> 0x4) & 1);
    reset = true;
}

void Envelope::tick() {
    if (reset) {
        val = 15;
        step = period;
        reset = false;
        return;
    }
    
    if (step) {
        step--;
    } else {
        step = period;
        
        if (val)
            val--;
        else if (loop)
            val = 15;
    }
}
