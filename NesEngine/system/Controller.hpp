//
//  Controller.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Controller_hpp
#define Controller_hpp

#include "common.h"

#include <map>

namespace nes::system {
    
    enum ControllerButton {
        A = 1 << 0,
        B = 1 << 1,
        Select = 1 << 2,
        Start = 1 << 3,
        Up = 1 << 4,
        Down = 1 << 5,
        Left = 1 << 6,
        Right = 1 << 7
    };
    
    class Controller {
    public:
        Controller();
        Byte read();
        void write(Byte b);
        void set(ControllerButton b, bool val);
        
        static Controller &controller1() {
            static Controller INST;
            return INST;
        }
        
        static Controller &controller2() {
            static Controller INST;
            return INST;
        }
        
    private:
        std::map<ControllerButton, bool> buttonStates;
        bool strobe;
        Byte data;
    };
    
}

#endif /* Controller_hpp */
