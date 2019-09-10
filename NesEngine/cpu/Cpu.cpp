//
//  Cpu.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Cpu.hpp"
#include "Log.hpp"
#include "DreamsysEmulator.hpp"

extern nes::system::DreamsysEmulator emulator;

#include <set>

using namespace nes::cpu;

std::set<const string> fetchCodes;

void initFetchCodes() {
    fetchCodes.clear();
    fetchCodes.insert("ADC");
    fetchCodes.insert("SBC");
    fetchCodes.insert("AND");
    fetchCodes.insert("ASL");
    fetchCodes.insert("BIT");
    fetchCodes.insert("CMP");
    fetchCodes.insert("CPX");
    fetchCodes.insert("CPY");
    fetchCodes.insert("DEC");
    fetchCodes.insert("EOR");
    fetchCodes.insert("INC");
    fetchCodes.insert("LDA");
    fetchCodes.insert("LDX");
    fetchCodes.insert("LDY");
    fetchCodes.insert("LSR");
    fetchCodes.insert("ORA");
    fetchCodes.insert("ROL");
    fetchCodes.insert("ROR");
}

Cpu::Cpu() {
    bus().setCpu(this);
    processor().setCpu(*this);
}

void Cpu::reset() {
    
    cpuData.pc = 0x8000;
    cpuData.sp = 0xFD;
    cpuData.regA = 0;
    cpuData.regX = 0;
    cpuData.regY = 0;
    cpuData.fetched = 0;
    cpuData.absoluteAddr = 0;
    cpuData.relativeAddr = 0;
    cpuData.regStatus = 0;
    
    setStatusFlag(Unused, 1);
    initFetchCodes();
    
    bus().reset();
    
    cpuData.absoluteAddr = 0xFFFC;
    cpuData.pc = bus().readWord(cpuData.absoluteAddr);
}

void Cpu::setStatusFlag(StatusFlag flag, bool value) {
    if (value) {
        cpuData.regStatus |= flag;
    } else {
        cpuData.regStatus &= ~flag;
    }
}

bool Cpu::getStatusFlag(StatusFlag flag) {
    return cpuData.regStatus & flag;
}

void Cpu::interrupt(InterruptType type) {
    
    if (type == BRK) {
        cpuData.pc++;
    }
    
    processor().stackPushWord(cpuData.pc);
    
    Byte sr = cpuData.regStatus;
    sr |= Unused;
    
    if (type == BRK) {
        sr |= Break;
    }
    
    processor().stackPush(sr);
    
    setStatusFlag(DisableInterrupt, true);
    
    cpuData.absoluteAddr = 0xFFFA;
    
    if (type == BRK) {
        cpuData.absoluteAddr = 0xFFFE;
    } else if (type == NMI) {
        
    }
    
    cpuData.pc = bus().readWord(cpuData.absoluteAddr);

    cpuData.cycles += 7;
}

void Cpu::fetchData(OpCode &opCode) {
    if (fetchCodes.find(opCode.name) == fetchCodes.end()) {
        return;
    }
    
    if (opCode.addressMode != IMP && opCode.addressMode != ACC) {
        cpuData.fetched = bus().read(cpuData.absoluteAddr);
    }
}

void Cpu::clockTick() {
    if (cpuData.cycles > 0) {
        cpuData.cycles--;
        return;
    }
    
    Address prev = cpuData.pc;
    
    Byte code = bus().read(cpuData.pc);
    OpCode opCode = opCodeLookup[code];
    opCodeLookup[code].code = code;
    cpuData.pc++;
    
    cpuData.cycles = opCode.cycles;
    
    Byte amc = memoryAddressor.fetch(*this, opCode);
    cpuData.fetched = cpuData.regA;
    
    fetchData(opCode);
    
    if (LOG_ENABLED) {
        LOG << hex64 << clockCount << " " << hex16 << prev << " " << opCode.name << " (" << hex8 << opCode.code << ") "
            << hex16 << cpuData.absoluteAddr << " " << hex8 << cpuData.fetched
        << " A: " << hex8 << cpuData.regA << " X: " << hex8 << cpuData.regX << " Y: " << hex8 << cpuData.regY
        << "CYC: " << std::dec << (int)emulator.getPpuData()->cycle << " P: " << hex8 << (int)emulator.getPpuData()->regStatus
            << endl;
    }
    
    clockCount++;
    
    Byte imc = processor().execute(opCode);
    
    cpuData.cycles += (amc & imc);
    
    cpuData.cycles--;
    
    if (intSet != NONE) {
        interrupt(intSet);
        intSet = NONE;
    }
}
