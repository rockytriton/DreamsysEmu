//
//  ChannelProcessors.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef ChannelProcessors_hpp
#define ChannelProcessors_hpp

#include "common.h"

namespace nes::apu {
    class PulseChannel;
    
    class Sweep {
    public:
        Sweep(PulseChannel *p) { ch = p; }
        void reload(Byte value);
        int tick();
        
    private:
        bool reset = false;
        bool enabled = false;
        Word val = 0;
        Byte period = 0;
        Byte shift = 0;
        Byte negate = 0;
        PulseChannel *ch;
    };
    
    class Envelope {
    public:
        void reload(Byte value);
        void tick();
        void doReset() { reset = true; }
        Byte getOutput() { return enabled ? val : period; }
    private:
        bool enabled = false;
        bool reset = false;
        Byte period = 0;
        bool loop = false;
        Byte step = 0;
        Byte val = 0;
    };
}

#endif /* ChannelProcessors_hpp */
