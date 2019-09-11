//
//  Cpu.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Cpu_hpp
#define Cpu_hpp

#include "common.h"
#include "OpCode.hpp"
#include "CpuBus.hpp"

#include "InstructionProcessor.hpp"
#include "MemoryAddressor.hpp"

namespace nes::cpu {
    
    enum InterruptType {
        NMI,
        BRK,
        IRQ,
        NONE
    };
    
    enum StatusFlag {
        Carry = 1 << 0,
        Zero = 1 << 1,
        DisableInterrupt = 1 << 2,
        Decimal = 1 << 3,
        Break = 1 << 4,
        Unused = 1 << 5,
        Overflow = 1 << 6,
        Negative = 1 << 7
    };
    
    class InstructionProcessor;
    class MemoryAddressor;
    
    struct CpuData {
        Byte regA;
        Byte regX;
        Byte regY;
        Address pc;
        Address sp;
        Word cycles;
        
        Byte regStatus;
        Byte fetched;
        Address absoluteAddr;
        Address relativeAddr;
    };
    
    class Cpu {
    public:
        Cpu();
        ~Cpu() {};
        
        void clockTick();
        
        void pause();
        void resume();
        void step();
        void reset();
        
        void setStatusFlag(StatusFlag flag, bool value);
        bool getStatusFlag(StatusFlag flag);
        
        CpuData &data() {
            return cpuData;
        }
        
        InstructionProcessor &processor() {
            return instProcessor;
        }
        
        CpuBus &bus() {
            return cpuBus;
        }
        
        void setInterrupt(InterruptType type) { intSet = type; }
        
        void skipDmaCycle() {
            Word ss = 513;
            ss += ((cpuData.cycles - 1) & 1);
            cpuData.cycles += ss;
        }
        bool interrupt(InterruptType type);
        
        uint64_t getClockCount() { return clockCount; }
        
        void clearDI() {
            createDisableInterrupt = true;
        }
    private:
        
        void fetchData(OpCode &opCode);
        
        InstructionProcessor instProcessor;
        MemoryAddressor memoryAddressor;
        
        CpuData cpuData;
        CpuBus cpuBus;
        
        bool paused;
        bool stepping;
        uint64_t clockCount;
        bool createDisableInterrupt = false;
        
        InterruptType intSet = NONE;
    };
    
}

#endif /* Cpu_hpp */
