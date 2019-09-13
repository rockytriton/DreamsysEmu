//
//  common.h
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/6/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef common_h
#define common_h

#ifdef __cplusplus

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>


typedef std::chrono::high_resolution_clock hrClock;
typedef std::chrono::duration<float, std::milli> msDuration;
typedef std::chrono::duration<float> sDuration;

typedef std::uint8_t Byte;
typedef std::uint16_t Address;
typedef std::uint16_t Word;

using std::vector;
using std::string;

#else

#include <stdint.h>

typedef uint8_t Byte;

#endif

#define USE_PORT_AUDIO 1

struct PixelData {
    Byte a;
    Byte r;
    Byte g;
    Byte b;
};

#define SAMPLE_RATE 44100

#define CPU_CLOCK_RATE 1789773
#define AUDIO_BUFFER_SIZE 44100

#define BETWEEN(a, b, c) a >= b && a <= c

#endif /* common_h */
