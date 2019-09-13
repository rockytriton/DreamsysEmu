//
//  InstructionProcessor.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef InstructionProcessor_hpp
#define InstructionProcessor_hpp

#include <map>

#include "OpCode.hpp"

namespace nes::cpu {
    class Cpu;
    
    typedef Byte (*handle_inst)(Cpu *, OpCode &);
    
    class InstructionProcessor {
    public:
        InstructionProcessor();
        ~InstructionProcessor();
        
        void setCpu(Cpu &cpu) {
            this->cpu = &cpu;
        }
        
        Byte execute(OpCode &opCode);
        
        void setNZFlags(Byte n);
        void setNZFlags16(Word n);
        
        void stackPush(Byte b);
        Byte stackPop();
        
        void stackPushWord(Word w);
        Word stackPopWord();
        
    private:
        
        void inst_add_handler(const string &name, handle_inst handler) {
            handlerMap.insert(std::pair<const std::string, handle_inst>(std::string(name), handler));
        }
        
        handle_inst inst_get_handler(const string &name) {
            std::map<const std::string, handle_inst>::iterator it = handlerMap.find(std::string(name));
            
            if (it != handlerMap.end()) {
                return it->second;
            }
            
            return nullptr;
        }
        
        Cpu *cpu;
        std::map<const std::string, handle_inst> handlerMap;
        handle_inst handlers[0x100];
    };
}

#endif /* InstructionProcessor_hpp */
