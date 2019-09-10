//
//  OpCode.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef OpCode_hpp
#define OpCode_hpp

#include "common.h"

namespace nes::cpu {
    enum AddressMode {
        IMM, IMP, INDX, INDY, ZP, ACC, ABS, ABSY, ABSX,
        REL, IND, ZPX, ZPY
    };
    
    struct OpCode {
        string name;
        Byte size;
        AddressMode addressMode;
        Byte cycles;
        Byte code;
    };
    
    extern OpCode opCodeLookup[];
}

#endif /* OpCode_hpp */
