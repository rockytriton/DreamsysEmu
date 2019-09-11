//
//  Apu.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Apu_hpp
#define Apu_hpp

#include "common.h"

#include "ApuChannel.hpp"
#include "Filters.hpp"

namespace nes::cpu {
    class Cpu;
}

namespace nes::apu {
    
    enum FCFlags {
        Mode1 = (1 << 7),
        DisableIrq = (1 << 6)
    };
    
    enum StatusFlags {
        DmcInterrupt = (1 << 7),
        FrameInterrupt = (1 << 6),
        EnableDmc = (1 << 4),
        Noise = (1 << 3),
        Triangle = (1 << 2),
        Pulse2 = (1 << 1),
        Pulse1 = (1 << 0)
    };
    
    class Mixer {
    public:
        Mixer();
        float sample(Byte pulse1, Byte pulse2, Byte triangle, Byte noise, Byte dmc);
        
    private:
        float pulseTable[31];
        float tndTable[203];
    };
    
    class Apu {
    public:
        Apu();
        virtual ~Apu();
        void clockTick();
        void write(Address address, Byte value);
        Byte read(Address address);
        void reset();
        void handleFrameCounter(Byte value);
        void setCpu(cpu::Cpu *p) { cpu = p; }
        void enable() { enabled = true; }
        bool isEnabled() { return enabled; }
        void readAudio(float *&samples, Word &len);
        float *readSamples(Word &len);
        
    protected:
        vector<ApuChannel *> channels;
        Byte regStatus;
        Byte regFrameCounter;
        Byte frameCount = 0;
        cpu::Cpu *cpu;
        bool enabled;
        uint32_t cycles = 0;
        uint32_t clockRate = 1789773;
        uint32_t sampleRate = 96000;
        Mixer mixer;
        float audioBuffer[4096] = {0};
        uint32_t audioIndex = 0;
        vector<Filter *> filters;
    };
    
}

#endif /* Apu_hpp */
