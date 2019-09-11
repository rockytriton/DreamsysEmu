//
//  SwiftBridge.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "SwiftBridge.hpp"
#include "common.h"
#include "Cartridge.hpp"
#include "DreamsysEmulator.hpp"
#include "Controller.hpp"

#include <unistd.h>

const int BUTTON_A = (1 << 0);
const int BUTTON_B = (1 << 1);
const int BUTTON_SE = (1 << 2);
const int BUTTON_ST = (1 << 3);
const int BUTTON_U = (1 << 4);
const int BUTTON_D = (1 << 5);
const int BUTTON_L = (1 << 6);
const int BUTTON_R = (1 << 7);

using namespace nes::system;

DreamsysEmulator emulator;

int loadCartridge(const char *fileName) {
    emulator.powerOff();
    usleep(500000);
    return emulator.loadRom(fileName) ? 1 : 0;
}

void emu_run() {
    emulator.run();
}

int emu_running() {
    return emulator.isRunning();
}

uint32_t **emu_image_buffer() {
    return emulator.getPpuData()->pictureBuffer;
}

unsigned int emu_current_frame() {
    return emulator.getPpuData()->curFrame;
}

void controller_set(int n, Byte b, Byte state) {
    Controller::controller1().set((ControllerButton)b, state);
}

int emu_read_audio(float *p) {
    uint16_t n = 0;
    
    emulator.readAudioBuffer(&p, &n);
    
    return n;
}

float *emu_read_samples(uint16_t *n) {
    return emulator.readSamples(n);
}
