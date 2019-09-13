//
//  ApuChannel.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef ApuChannel_hpp
#define ApuChannel_hpp

#include "common.h"

#include "ChannelProcessors.hpp"

namespace nes::apu {
    class ApuChannel {
    public:
        virtual ~ApuChannel() {}
        virtual void write(Byte reg, Byte value);
        virtual Byte read(Byte reg) = 0;
        virtual void silence();
        virtual void wake() { silenced = false; }
        virtual Byte counter() { return lengthCounter; }
        virtual void sweep();
        virtual void tick(bool even) = 0;
        virtual bool updateTimer();
        virtual void envelope() = 0;
        Word getTimerPeriod() { return timerPeriod; }
        virtual Byte output() { return 0; }
        
    protected:
        Byte registers[4] = {0,0,0,0};
        Byte lengthCounter = 0;
        Word timer = 0;
        Word timerPeriod = 0;
        Word duty = 0;
        Envelope env;
        Byte dutyCycle = 0;
        
        bool silenced = false;
        bool counterEnabled = false;
    };
    
    class PulseChannel : public ApuChannel {
    public:
        PulseChannel(int ch) : sweeper(this) { pChan = ch; }
        virtual ~PulseChannel() {}
        virtual void write(Byte reg, Byte value);
        virtual Byte read(Byte reg);
        virtual void envelope();
        virtual void sweep();
        virtual void tick(bool even);
        int getChan() { return pChan; }
        virtual Byte output();
        
    private:
        Sweep sweeper;
        int pChan;
        Byte sequencerIndex = 0;
    };
    
    class TriangleChannel : public ApuChannel {
    public:
        virtual ~TriangleChannel() {}
        virtual void write(Byte reg, Byte value);
        virtual Byte read(Byte reg);
        virtual void envelope();
        virtual void tick(bool even);
        virtual Byte output();
        
    private:
        bool linReset = false;
        Byte linVal = 0;
        Byte linPeriod = 0;
        bool linOn = false;
    };
    
    class NoiseChannel : public ApuChannel {
    public:
        virtual ~NoiseChannel() {}
        virtual void write(Byte reg, Byte value);
        virtual Byte read(Byte reg);
        virtual void envelope();
        virtual void tick(bool even);
        virtual Byte output() { return (silenced || lengthCounter == 0 || (sr & 1) == 1) ? 0 : env.getOutput(); }
        
    private:
        bool mode = false;
        Word sr = 1;
    };
    
    class DmcChannel : public ApuChannel {
    public:
        virtual ~DmcChannel() {}
        virtual void write(Byte reg, Byte value);
        virtual Byte read(Byte reg);
        virtual void envelope();
        virtual void tick(bool even);
        
    private:
        Address currentAddress = 0;
        Address sampleAddress = 0;
        Word sampleLength = 0;
        Byte sampleBuffer = 0;
        
        Byte brPeriod = 8;
        Byte brLoop = false;
        Byte timerLoop = true;
        
        Byte shiftRegister = 0;
        Byte outputLevel = 0;
        bool irqEnabled = false;
    };
}

#endif /* ApuChannel_hpp */
