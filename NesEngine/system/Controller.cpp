//
//  Controller.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Controller.hpp"

using namespace nes::system;

Controller::Controller() {
    buttonStates[A] = false;
    buttonStates[B] = false;
    buttonStates[Select] = false;
    buttonStates[Start] = false;
    buttonStates[Up] = false;
    buttonStates[Down] = false;
    buttonStates[Left] = false;
    buttonStates[Right] = false;
    strobe = false;
    data = 0;
}

Byte Controller::read() {
    if (strobe) {
        return data & ControllerButton::A;
    }
    
    Byte v = data & 1;
    data >>= 1;
    
    return v | 0x40;
}

void Controller::write(Byte b) {
    strobe = b & 1;
    
    if (!strobe) {
        Byte keyStates = 0;
        int shift = 0;
        
        for (std::map<ControllerButton, bool>::iterator it = buttonStates.begin(); it != buttonStates.end(); it++) {
            keyStates |= ((*it).second << shift);
            shift++;
        }
        
        data = keyStates;
    }
}

void Controller::set(ControllerButton b, bool val) {
    if (val) {
        data |= b;
    } else {
        data &= ~b;
    }
    
    buttonStates[b] = val;
}
