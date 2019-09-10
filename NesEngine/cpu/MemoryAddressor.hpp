//
//  MemoryAddressing.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef MemoryAddressing_hpp
#define MemoryAddressing_hpp

#include "common.h"
#include "OpCode.hpp"

namespace nes::cpu {
    
    class Cpu;
    
    class MemoryAddressor {
    public:
        MemoryAddressor();
        
        Byte fetch(Cpu &cpu, OpCode &opCode);
        
    private:
        Byte fetchAbsAddr(Cpu &cpu, Byte increment);
    };
}

#endif /* MemoryAddressing_hpp */
