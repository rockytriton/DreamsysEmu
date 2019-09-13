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

#include "PortAudio/portaudio.h"

#include <unistd.h>

const int BUTTON_A = (1 << 0);
const int BUTTON_B = (1 << 1);
const int BUTTON_SE = (1 << 2);
const int BUTTON_ST = (1 << 3);
const int BUTTON_U = (1 << 4);
const int BUTTON_D = (1 << 5);
const int BUTTON_L = (1 << 6);
const int BUTTON_R = (1 << 7);

PaStream *audioStream;
PaError audioError;

using namespace nes::system;

DreamsysEmulator emulator;

int loadCartridge(const char *fileName) {
    emulator.powerOff();
    usleep(500000);
    return emulator.loadRom(fileName) ? 1 : 0;
}

Byte emu_read_address(Address addr) {
    return emulator.read(addr);
}

void updateAudio() {
    uint16_t n = 0;
    
    float *f = emu_read_samples(&n);
    Pa_WriteStream(audioStream, f, n);
    emulator.resetBuffer();
    //nes->audioBufferLength = 0;
}


void emu_init() {
    
    audioError = Pa_Initialize();
    if (audioError != paNoError) {
        printf("Error initalizing Audio\n");
        exit(0);
    }
    
    PaStreamParameters output;
    output.device = Pa_GetDefaultOutputDevice();
    output.channelCount = 1;
    output.sampleFormat = paFloat32;
    output.suggestedLatency = Pa_GetDeviceInfo(output.device)->defaultHighOutputLatency;
    output.hostApiSpecificStreamInfo = NULL;
    
    Pa_OpenStream(&audioStream,
                  NULL,
                  &output,
                  44100,
                  735,
                  paClipOff,
                  NULL,
                  NULL);
}

void emu_run() {
    Pa_StartStream(audioStream);
    emulator.run();
    Pa_StopStream(audioStream);
}

int emu_running() {
    return emulator.isRunning();
}

uint32_t **emu_image_buffer() {
    return emulator.getPpuData()->pictureBuffer;
}

extern "C" PixelData **emu_buffer() {
    return emulator.getPpuData()->pixelBuffer;
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

void emu_done_read() {
    emulator.resetBuffer();
}

Byte *emu_read_ram() {
    return emulator.readRam();
}
