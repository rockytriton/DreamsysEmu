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
        
    };
}

#endif /* ApuChannel_hpp */
