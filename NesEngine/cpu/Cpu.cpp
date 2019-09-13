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
#include "MapperMMC3.hpp"

extern nes::system::DreamsysEmulator emulator;

#include <set>

using namespace nes::cpu;

std::set<const string> fetchCodes;
bool fetchCodeLookup[0x100];

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
    
    for (int i=0; i<0x100; i++) {
        if (fetchCodes.find(opCodeLookup[i].name) != fetchCodes.end()) {
            fetchCodeLookup[i] = true;
        } else {
            fetchCodeLookup[i] = false;
        }
    }
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

/*
 #define    _IRQ() {            \
 PUSH( R.PC>>8 );        \
 PUSH( R.PC&0xFF );        \
 CLR_FLAG( B_FLAG );        \
 PUSH( R.P );            \
 SET_FLAG( I_FLAG );        \
 R.PC = RD6502W(IRQ_VECTOR);    \
 exec_cycles += 7;        \
 */

bool Cpu::interrupt(InterruptType type) {
    
    if (getStatusFlag(DisableInterrupt) && (type != NMI && type != BRK)) {
        //LOG << "SKIPPING INT DISABLED" << endl;
        
        if (createDisableInterrupt) {
            createDisableInterrupt = false;
            setStatusFlag(DisableInterrupt, false);
        }
        
        return false;
    }
    
    //LOG << "Running Interrupt" << endl;
    //printf("RUNNING INTERRUPT: %d\r\n", type);
    
    if (type == BRK) {
        cpuData.pc++;
    }
    
    processor().stackPushWord(cpuData.pc);
    
    Byte sr = cpuData.regStatus;
    
    sr |= Unused;
    
    if (type == BRK) {
        sr |= Break;
        setStatusFlag(Break, true);
    }
    
    processor().stackPush(sr);
    
    setStatusFlag(DisableInterrupt, true);
    
    cpuData.absoluteAddr = 0xFFFA;
    
    if (type == BRK || type == IRQ) {
        cpuData.absoluteAddr = 0xFFFE;
    } else if (type == NMI) {
        
    }
    
    cpuData.pc = cpuBus.readWord(cpuData.absoluteAddr);
    
    //LOG << "SET PC ADDR: " << hex16 << cpuData.pc << endl;

    cpuData.cycles += 7;
    
    return true;
}

void Cpu::fetchData(OpCode &opCode) {
    if (!fetchCodeLookup[opCode.code]) {
        return;
    }
    
    if (opCode.addressMode != IMP && opCode.addressMode != ACC) {
        cpuData.fetched = cpuBus.read(cpuData.absoluteAddr);
    }
}

extern int startLogging;

void Cpu::clockTick() {
    if (cpuData.cycles > 0) {
        cpuData.cycles--;
        return;
    }
    
    Address prev = cpuData.pc;
    
    Byte code = cpuBus.read(cpuData.pc);
    OpCode opCode = opCodeLookup[code];
    opCodeLookup[code].code = code;
    cpuData.pc++;
    
    cpuData.cycles = opCode.cycles;
    
    Byte amc = memoryAddressor.fetch(*this, opCode);
    cpuData.fetched = cpuData.regA;
    
    //if (opCode.name == "CLI") {
    //    startLogging = 1;
    //}
    
    fetchData(opCode);
    
    if (LOG_ENABLED && startLogging && 0) {
        LOG << hex64 << clockCount << " " << hex16 << prev << " " << opCode.name << " (" << hex8 << opCode.code << ") "
            << hex16 << cpuData.absoluteAddr << " " << hex8 << cpuData.fetched
        << " A: " << hex8 << cpuData.regA << " X: " << hex8 << cpuData.regX << " Y: " << hex8 << cpuData.regY
        << " CYC: " << std::dec << (int)emulator.getPpuData()->cycle << " P: " << hex8 << (int)emulator.getPpuData()->regStatus
            << endl;
    }
    
    clockCount++;
    
    Byte imc = processor().execute(opCode);
    
    if (intSet != NONE) {
        if (interrupt(intSet)) {
            intSet = NONE;
        }
    }
    
    cpuData.cycles += (amc & imc);
    
    cpuData.cycles--;
}
