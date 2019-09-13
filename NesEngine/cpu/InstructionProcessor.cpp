//
//  InstructionProcessor.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "InstructionProcessor.hpp"
#include "Log.hpp"
#include "Cpu.hpp"

using namespace nes::cpu;

#define DEF_F(x) Byte x(Cpu *cpu, OpCode &opCode)

int startLogging = 0;



DEF_F(handle_SEI) {
    cpu->setStatusFlag(DisableInterrupt, true);
    return 0;
}

DEF_F(handle_JMP) {
    cpu->data().pc = cpu->data().absoluteAddr;
    return 0;
}

DEF_F(handle_BRK) {
    cpu->interrupt(BRK);
    return 0;
}


DEF_F(handle_SEC) {
    cpu->setStatusFlag(Carry, 1);
    return 0;
}

DEF_F(handle_SED) {
    cpu->setStatusFlag(Decimal, 1);
    return 0;
}

DEF_F(handle_CLC) {
    cpu->setStatusFlag(Carry, 0);
    return 0;
}

DEF_F(handle_CLD) {
    cpu->setStatusFlag(Decimal, 0);
    return 0;
}

DEF_F(handle_CLI) {
    //cpu->setStatusFlag(DisableInterrupt, 0);
    cpu->clearDI();
    return 0;
}

DEF_F(handle_CLV) {
    cpu->setStatusFlag(nes::cpu::Overflow, 0);
    return 0;
}

DEF_F(handle_LDA) {
    cpu->data().regA = cpu->data().fetched;
    cpu->processor().setNZFlags(cpu->data().regA);
    return 1;
}

DEF_F(handle_LDX) {
    cpu->data().regX = cpu->data().fetched;
    cpu->processor().setNZFlags(cpu->data().regX);
    return 1;
}

DEF_F(handle_LDY) {
    cpu->data().regY = cpu->data().fetched;
    cpu->processor().setNZFlags(cpu->data().regY);
    return 1;
}

DEF_F(handle_STA) {
    cpu->bus().write(cpu->data().absoluteAddr, cpu->data().regA);
    return 0;
}

DEF_F(handle_STX) {
    cpu->bus().write(cpu->data().absoluteAddr, cpu->data().regX);
    return 0;
}

DEF_F(handle_STY) {
    cpu->bus().write(cpu->data().absoluteAddr, cpu->data().regY);
    return 0;
}

DEF_F(handle_AND) {
    cpu->data().regA = cpu->data().regA & cpu->data().fetched;
    cpu->processor().setNZFlags(cpu->data().regA);
    return 1;
}

DEF_F(handle_BEQ) {
    if (cpu->getStatusFlag(Zero)) {
        cpu->data().cycles++;
        cpu->data().absoluteAddr = cpu->data().pc + cpu->data().relativeAddr;
        
        if ((cpu->data().absoluteAddr & 0xFF00) != ((cpu->data().pc) & 0xFF00)) {
            cpu->data().cycles++;
            cpu->data().cycles++;
        }
        
        cpu->data().pc = cpu->data().absoluteAddr;
    }
    
    return 0;
}

DEF_F(handle_DEX) {
    cpu->data().regX--;
    cpu->processor().setNZFlags(cpu->data().regX);
    return 0;
}

DEF_F(handle_DEY) {
    cpu->data().regY--;
    cpu->processor().setNZFlags(cpu->data().regY);
    return 0;
}

DEF_F(handle_TAX) {
    cpu->data().regX = cpu->data().regA;
    cpu->processor().setNZFlags(cpu->data().regX);
    return 0;
}

DEF_F(handle_TAY) {
    cpu->data().regY = cpu->data().regA;
    cpu->processor().setNZFlags(cpu->data().regY);
    return 0;
}

DEF_F(handle_TSX) {
    cpu->data().regX = cpu->data().sp;
    cpu->processor().setNZFlags(cpu->data().regX);
    return 0;
}

DEF_F(handle_TXA) {
    cpu->data().regA = cpu->data().regX;
    cpu->processor().setNZFlags(cpu->data().regA);
    return 0;
}

DEF_F(handle_TXS) {
    cpu->data().sp = cpu->data().regX;
    return 0;
}

DEF_F(handle_TYA) {
    cpu->data().regA = cpu->data().regY;
    cpu->processor().setNZFlags(cpu->data().regA);
    return 0;
}

DEF_F(handle_BIT) {
    uint16_t temp = cpu->data().regA & cpu->data().fetched;
    cpu->setStatusFlag(Zero, (temp & 0x00FF) == 0x00);
    cpu->setStatusFlag(Negative, cpu->data().fetched & (1 << 7));
    cpu->setStatusFlag(nes::cpu::Overflow, cpu->data().fetched & (1 << 6));
    return 0;
}

DEF_F(handle_BPL) {
    if (!cpu->getStatusFlag(Negative)) {
        
        cpu->data().cycles++;
        cpu->data().absoluteAddr = (cpu->data().pc) + cpu->data().relativeAddr;
        
        if ((cpu->data().absoluteAddr & 0xFF00) != ((cpu->data().pc) & 0xFF00)) {
            cpu->data().cycles++;
        }
        
        cpu->data().pc = cpu->data().absoluteAddr;
    } else {
        return 0;
    }
    return 0;
}

DEF_F(handle_BNE) {
    if (!cpu->getStatusFlag(Zero)) {
        cpu->data().cycles++;
        cpu->data().absoluteAddr = (cpu->data().pc) + cpu->data().relativeAddr;
        
        if ((cpu->data().absoluteAddr & 0xFF00) != ((cpu->data().pc) & 0xFF00)) {
            cpu->data().cycles++;
        }
        
        cpu->data().pc = cpu->data().absoluteAddr;
    }
    
    return 0;
}

DEF_F(handle_CMP) {
    uint16_t temp = (uint16_t)cpu->data().regA - (uint16_t)cpu->data().fetched;
    cpu->setStatusFlag(Carry, cpu->data().regA >= cpu->data().fetched);
    cpu->processor().setNZFlags16(temp);
    return 1;
}

DEF_F(handle_JSR) {
    cpu->data().pc--;
    
    cpu->processor().stackPushWord(cpu->data().pc);
    
    cpu->data().pc = cpu->data().absoluteAddr;
    
    return 0;
}

DEF_F(handle_DEC) {
    uint16_t temp = cpu->data().fetched - 1;
    cpu->bus().write(cpu->data().absoluteAddr, temp & 0x00FF);
    cpu->processor().setNZFlags16(temp);
    
    return 0;
}

DEF_F(handle_CPY) {
    uint16_t temp = (uint16_t)cpu->data().regY - (uint16_t)cpu->data().fetched;
    cpu->setStatusFlag(Carry, cpu->data().regY >= cpu->data().fetched);
    cpu->processor().setNZFlags16(temp);
    return 0;
}

DEF_F(handle_CPX) {
    uint16_t temp = (uint16_t)cpu->data().regX - (uint16_t)cpu->data().fetched;
    cpu->setStatusFlag(Carry, cpu->data().regX >= cpu->data().fetched);
    cpu->processor().setNZFlags16(temp);
    return 0;
}

DEF_F(handle_ORA) {
    cpu->data().regA |= cpu->data().fetched;
    cpu->processor().setNZFlags(cpu->data().regA);
    return 1;
}

DEF_F(handle_PHA) {
    cpu->processor().stackPush(cpu->data().regA);
    return 0;
}

DEF_F(handle_PHP) {
    cpu->processor().stackPush(cpu->data().regStatus | Break | Unused);
    cpu->setStatusFlag(Break, 0);
    cpu->setStatusFlag(Unused, 0);
    
    return 0;
}

DEF_F(handle_PLA) {
    cpu->data().regA = cpu->processor().stackPop();
    cpu->processor().setNZFlags(cpu->data().regA);
    return 0;
}

DEF_F(handle_PLP) {
    cpu->data().regStatus = cpu->processor().stackPop();
    cpu->setStatusFlag(Unused, 1);
    return 0;
}

DEF_F(handle_INC) {
    uint16_t temp = cpu->data().fetched + 1;
    cpu->bus().write(cpu->data().absoluteAddr, temp & 0x00FF);
    cpu->processor().setNZFlags16(temp);
    return 0;
}

DEF_F(handle_INX) {
    cpu->data().regX++;
    cpu->processor().setNZFlags(cpu->data().regX);
    return 0;
}

DEF_F(handle_INY) {
    cpu->data().regY++;
    cpu->processor().setNZFlags(cpu->data().regY);
    return 0;
}

DEF_F(handle_ROL) {
    if (opCode.addressMode == ACC)
    {
        bool prev_C = cpu->getStatusFlag(Carry);
        cpu->setStatusFlag(Carry, cpu->data().regA & 0x80);
        cpu->data().regA <<= 1;
        //If Rotating, set the bit-0 to the the previous carry
        cpu->data().regA = cpu->data().regA | (prev_C && (1));
        cpu->processor().setNZFlags(cpu->data().regA);
    }
    else
    {
        bool prev_C = cpu->getStatusFlag(Carry);
        cpu->data().fetched = cpu->bus().read(cpu->data().absoluteAddr);
        cpu->setStatusFlag(Carry, cpu->data().fetched & 0x80);
        cpu->data().fetched = cpu->data().fetched << 1 | (prev_C && (1));
        cpu->processor().setNZFlags(cpu->data().fetched);
        cpu->bus().write(cpu->data().absoluteAddr, cpu->data().fetched);
    }
    
    return 0;
}

DEF_F(handle_ROR) {
    if (opCode.addressMode == ACC)
    {
        bool prev_C = cpu->getStatusFlag(Carry);
        cpu->setStatusFlag(Carry,  cpu->data().regA & 1);
        cpu->data().regA >>= 1;
        
        cpu->data().regA = cpu->data().regA | (prev_C && (1)) << 7;
        cpu->processor().setNZFlags(cpu->data().regA);
    }
    else
    {
        bool prev_C = cpu->getStatusFlag(Carry);
        Byte operand = cpu->bus().read(cpu->data().absoluteAddr);
        cpu->setStatusFlag(Carry, operand & 1);
        operand = operand >> 1 | (prev_C && 1) << 7;
        
        cpu->processor().setNZFlags(operand);
        cpu->bus().write(cpu->data().absoluteAddr, operand);
    }
    
    return 0;
}

DEF_F(handle_RTI) {
    cpu->data().regStatus = cpu->processor().stackPop();
    cpu->data().regStatus &= ~Break;
    cpu->data().regStatus &= ~Unused;
    
    cpu->data().pc = cpu->processor().stackPopWord();
    
    return 0;
}

DEF_F(handle_RTS) {
    cpu->data().pc = cpu->processor().stackPopWord();
    cpu->data().pc++;
    
    return 0;
}

DEF_F(handle_ADC) {
    uint16_t temp = (uint16_t)cpu->data().regA + (uint16_t)(cpu->data().fetched) + (uint16_t)(cpu->getStatusFlag(Carry));
    cpu->setStatusFlag(Carry, temp > 255);
    cpu->setStatusFlag(Zero, (temp & 0x00FF) == 0);
    cpu->setStatusFlag(nes::cpu::Overflow, (~((uint16_t)cpu->data().regA ^ (uint16_t)cpu->data().fetched) & ((uint16_t)cpu->data().regA ^ (uint16_t)temp)) & 0x0080);
    
    cpu->setStatusFlag(Negative, temp & 0x80);
    
    cpu->data().regA = temp & 0x00FF;
    
    return 1;
}

DEF_F(handle_SBC) {
    
    uint16_t subtrahend = cpu->bus().read(cpu->data().absoluteAddr);
    uint16_t diff = cpu->data().regA - subtrahend - !cpu->getStatusFlag(Carry);
    
    cpu->setStatusFlag(Carry, !(diff & 0x100));
    
    cpu->setStatusFlag(nes::cpu::Overflow, (cpu->data().regA ^ diff) & (~subtrahend ^ diff) & 0x80);
    cpu->data().regA = diff;
    cpu->processor().setNZFlags(diff);
    
    return 1;
}

DEF_F(handle_LSR) {
    cpu->setStatusFlag(Carry, cpu->data().fetched & 1);
    uint16_t temp = cpu->data().fetched >> 1;
    cpu->processor().setNZFlags16(temp);
    
    
    if (opCode.addressMode == ACC) {
        cpu->data().regA = temp & 0x00FF;
    } else {
        cpu->bus().write(cpu->data().absoluteAddr, temp & 0x00FF);
    }
    
    return 0;
}

DEF_F(handle_ASL) {
    uint16_t temp = (uint16_t)cpu->data().fetched << 1;
    cpu->setStatusFlag(Carry, (temp & 0xFF00) > 0);
    cpu->processor().setNZFlags16(temp);
    
    if (opCode.addressMode == ACC) {
        cpu->data().regA = temp & 0x00FF;
    } else {
        cpu->bus().write(cpu->data().absoluteAddr, temp & 0x00FF);
    }
    return 0;
}
int onBcc = 0;
DEF_F(handle_BCC) {
    onBcc = 1;
    if (!cpu->getStatusFlag(Carry)) {
        cpu->data().cycles++;
        cpu->data().absoluteAddr = cpu->data().pc + cpu->data().relativeAddr;
        
        if ((cpu->data().absoluteAddr & 0xFF00) != (cpu->data().pc & 0xFF00)) {
            cpu->data().cycles++;
        }
        
        cpu->data().pc = cpu->data().absoluteAddr;
    }
    return 0;
}

DEF_F(handle_BCS) {
    if (cpu->getStatusFlag(Carry)) {
        cpu->data().cycles++;
        cpu->data().absoluteAddr = cpu->data().pc + cpu->data().relativeAddr;
        
        if ((cpu->data().absoluteAddr & 0xFF00) != (cpu->data().pc & 0xFF00)) {
            cpu->data().cycles++;
        }
        
        cpu->data().pc = cpu->data().absoluteAddr;
    }
    return 0;
}

DEF_F(handle_BMI) {
    if (cpu->getStatusFlag(Negative)) {
        cpu->data().cycles++;
        cpu->data().absoluteAddr = cpu->data().pc + cpu->data().relativeAddr;
        
        if ((cpu->data().absoluteAddr & 0xFF00) != (cpu->data().pc & 0xFF00)) {
            cpu->data().cycles++;
        }
        
        cpu->data().pc = cpu->data().absoluteAddr;
    }
    
    return 0;
}

DEF_F(handle_EOR) {
    cpu->data().regA = cpu->data().regA ^ cpu->data().fetched;
    cpu->processor().setNZFlags(cpu->data().regA);
    return 1;
}

DEF_F(handle_NOP) {
    return 0;
}

DEF_F(handle_BVC) {
    if (!cpu->getStatusFlag(nes::cpu::Overflow)) {
        cpu->data().cycles++;
        cpu->data().absoluteAddr = cpu->data().pc + cpu->data().relativeAddr;
        
        
        if ((cpu->data().absoluteAddr & 0xFF00) != (cpu->data().pc & 0xFF00))
            cpu->data().cycles++;
        
        cpu->data().pc = cpu->data().absoluteAddr;
    }
    return 0;
}

DEF_F(handle_BVS) {
    if (cpu->getStatusFlag(nes::cpu::Overflow)) {
        cpu->data().cycles++;
        cpu->data().absoluteAddr = cpu->data().pc + cpu->data().relativeAddr;
        
        if ((cpu->data().absoluteAddr & 0xFF00) != (cpu->data().pc & 0xFF00))
            cpu->data().cycles++;
        
        cpu->data().pc = cpu->data().absoluteAddr;
    }
    return 0;
}


InstructionProcessor::InstructionProcessor() {
    
    
    inst_add_handler("JMP", handle_JMP);
    inst_add_handler("BRK", handle_BRK);
    inst_add_handler("SEI", handle_SEI);
    inst_add_handler("SEC", handle_SEC);
    inst_add_handler("SED", handle_SED);
    inst_add_handler("CLC", handle_CLC);
    inst_add_handler("CLI", handle_CLI);
    inst_add_handler("CLV", handle_CLV);
    inst_add_handler("CLD", handle_CLD);
    inst_add_handler("LDA", handle_LDA);
    inst_add_handler("LDX", handle_LDX);
    inst_add_handler("LDY", handle_LDY);
    inst_add_handler("STA", handle_STA);
    inst_add_handler("STX", handle_STX);
    inst_add_handler("STY", handle_STY);
    inst_add_handler("AND", handle_AND);
    inst_add_handler("BEQ", handle_BEQ);
    inst_add_handler("BCS", handle_BCS);
    inst_add_handler("JSR", handle_JSR);
    
    inst_add_handler("DEC", handle_DEC);
    inst_add_handler("DEX", handle_DEX);
    inst_add_handler("DEY", handle_DEY);
    inst_add_handler("TAX", handle_TAX);
    inst_add_handler("TAY", handle_TAY);
    inst_add_handler("TSX", handle_TSX);
    inst_add_handler("TXA", handle_TXA);
    inst_add_handler("TXS", handle_TXS);
    inst_add_handler("TYA", handle_TYA);
    inst_add_handler("BIT", handle_BIT);
    inst_add_handler("BPL", handle_BPL);
    inst_add_handler("BNE", handle_BNE);
    inst_add_handler("CMP", handle_CMP);
    inst_add_handler("CPY", handle_CPY);
    inst_add_handler("CPX", handle_CPX);
    inst_add_handler("ORA", handle_ORA);
    inst_add_handler("PHA", handle_PHA);
    inst_add_handler("PHP", handle_PHP);
    inst_add_handler("PLA", handle_PLA);
    inst_add_handler("PLP", handle_PLP);
    inst_add_handler("INC", handle_INC);
    inst_add_handler("INX", handle_INX);
    inst_add_handler("INY", handle_INY);
    inst_add_handler("ROL", handle_ROL);
    inst_add_handler("ROR", handle_ROR);
    inst_add_handler("RTI", handle_RTI);
    inst_add_handler("RTS", handle_RTS);
    inst_add_handler("ADC", handle_ADC);
    inst_add_handler("SBC", handle_SBC);
    inst_add_handler("LSR", handle_LSR);
    inst_add_handler("ASL", handle_ASL);
    inst_add_handler("BCC", handle_BCC);
    inst_add_handler("BMI", handle_BMI);
    inst_add_handler("EOR", handle_EOR);
    inst_add_handler("NOP", handle_NOP);
    inst_add_handler("BVC", handle_BVC);
    inst_add_handler("BVS", handle_BVS);
    
    for (int i=0; i<=0xFF; i++) {
        handlers[i] = inst_get_handler(opCodeLookup[i].name);
        opCodeLookup[i].code = i;
    }
}

InstructionProcessor::~InstructionProcessor() {
    
}


void InstructionProcessor::setNZFlags(Byte n) {
    cpu->setStatusFlag(Zero, n == 0);
    cpu->setStatusFlag(Negative, n & 0x80);
}

void InstructionProcessor::setNZFlags16(Word n) {
    cpu->setStatusFlag(Zero, (n & 0x00FF) == 0x0000);
    cpu->setStatusFlag(Negative, n & 0x0080);
}

Byte InstructionProcessor::execute(OpCode &opCode) {
    handle_inst f = handlers[opCode.code]; //inst_get_handler(opCode.name);
    
    if (f) {
        return f(cpu, opCode);
    } else {
        LOG << hex64 << cpu->getClockCount() << " INST: " << opCode.name << endl;
        
        return 0;
    }
    
    return 0;
}


void InstructionProcessor::stackPush(Byte b) {
    cpu->bus().write(cpu->data().sp + 0x0100, b);
    cpu->data().sp--;
}

Byte InstructionProcessor::stackPop() {
    cpu->data().sp++;
    return cpu->bus().read(cpu->data().sp + 0x0100);
}

void InstructionProcessor::stackPushWord(Word w) {
    cpu->processor().stackPush((w >> 8) & 0x00FF);
    cpu->processor().stackPush(w & 0x00FF);
}

Word InstructionProcessor::stackPopWord() {
    Word hi = cpu->processor().stackPop();
    Word lo = cpu->processor().stackPop();
    
    return (lo << 8) | hi;
}
