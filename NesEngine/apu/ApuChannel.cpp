//
//  ApuChannel.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "ApuChannel.hpp"

using namespace nes::apu;

void ApuChannel::silence() {
    lengthCounter = 0;
    silenced = true;
}

void ApuChannel::write(Byte reg, Byte value) {
    if (reg == 0) {
        env.reload(value);
        counterEnabled = !(value & 0x20);
        duty = value >> 6; //(value & 0xC0) >> 6;
    }
}

void ApuChannel::sweep() {
    if (counterEnabled && lengthCounter) {
        lengthCounter--;
    }
}

bool ApuChannel::updateTimer() {
    if (timer) {
        timer--;
    } else {
        timer = timerPeriod;
        
        return true;
    }
    
    return false;
}

static constexpr Byte LEN_COUNTER_TABLE [] = {
    10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
    12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};

static constexpr bool DUTY_CYCLE_TABLE [][8] = {
    { 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0 },
    { 1, 0, 0, 1, 1, 1, 1, 1 }
};
uint8_t dutyTable[4][8] = {
    { 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 0, 0, 0, 0, 0 },
    { 0, 1, 1, 1, 1, 0, 0, 0 },
    { 1, 0, 0, 1, 1, 1, 1, 1 }
};

Byte PulseChannel::output() {
    bool active = DUTY_CYCLE_TABLE[duty][sequencerIndex];

    if (silenced || !active || !lengthCounter || timer < 8 || timerPeriod > 0x7FFF) {
        return 0;
    }
    
    return env.getOutput();
}

void PulseChannel::write(Byte reg, Byte value) {
    ApuChannel::write(reg, value);
    
    if (reg == 0) {
        //duty = (value & 0xC0) >> 6;
    } else if (reg == 1) {
        sweeper.reload(value);
    } else if (reg == 2) {
        timerPeriod = (timerPeriod & 0xFF00) | value;
    } else if (reg == 3) {
        timerPeriod = (timerPeriod & 0x00FF) | ((value & 7) << 8);
        
        lengthCounter = LEN_COUNTER_TABLE[(value & 0xF8) >> 3];
        env.doReset();
        sequencerIndex = 0;
    }
}

Byte PulseChannel::read(Byte reg) {
    return 0;
}

void PulseChannel::envelope() {
    env.tick();
}

void PulseChannel::sweep() {
    ApuChannel::sweep();
    timerPeriod += sweeper.tick();
}

void PulseChannel::tick(bool even) {
    if (even) {
        return;
    }
    
    if (updateTimer()) {
        sequencerIndex = (sequencerIndex + 1) % 8;
    }
}

static constexpr Byte TRIANGLE_TABLE [] = {
    15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
};

Byte TriangleChannel::output() {
    if (silenced || !lengthCounter || !linVal) {
        return 0;
    }
    
    return TRIANGLE_TABLE[duty];
}


void TriangleChannel::write(Byte reg, Byte value) {
    ApuChannel::write(reg, value);
    
    if (reg == 0) {
        linOn = !(value & 0x80);
        counterEnabled = linOn;
        linPeriod = value & 0x7F;
    } else if (reg == 2) {
        timerPeriod = (timerPeriod & 0xFF00) | value;
    } else if (reg == 3) {
        timerPeriod = (timerPeriod & 0x00FF) | ((value & 7) << 8);
        
        lengthCounter = LEN_COUNTER_TABLE[(value & 0xF8) >> 3];
        linReset = true;
        timer = timerPeriod;
    }
}

Byte TriangleChannel::read(Byte reg) {
    return 0;
}

void TriangleChannel::envelope() {
    if (linReset) {
        linVal = linPeriod;
    } else if (linVal) {
        linVal--;
    }
    
    if (linOn) {
        linReset = false;
    }
}

void TriangleChannel::tick(bool even) {
    if (updateTimer()) {
        duty = (duty + 1) % 32;
    }
}

static constexpr Word NOISE_PERIOD_TABLE [] = {
    4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
};

void NoiseChannel::write(Byte reg, Byte value) {
    ApuChannel::write(reg, value);
    
    if (reg == 2) {
        mode = (value & 0x80);
        timerPeriod = NOISE_PERIOD_TABLE[value & 0x0F];
    } else if (reg == 3) {
        lengthCounter = LEN_COUNTER_TABLE[(value & 0xF8) >> 3];
        env.doReset();
    }
    
}

Byte NoiseChannel::read(Byte reg) {
    return 0;
}

void NoiseChannel::envelope() {
    env.tick();
}

void NoiseChannel::tick(bool even) {
    if (updateTimer()) {
        
        bool fb = (sr & 1) ^ (sr & (mode ? (1 << 6) : (1 << 1)));
        sr >>= 1;
        sr |= fb << 14;
    }
}

void DmcChannel::write(Byte reg, Byte value) {
    ApuChannel::write(reg, value);
    
    
}

Byte DmcChannel::read(Byte reg) {
    return 0;
}

void DmcChannel::envelope() {
    
}

void DmcChannel::tick(bool even) {
    
}
