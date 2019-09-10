//
//  Timer.cpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#include "Timer.hpp"

using namespace nes::system;

Timer::Timer() {
    cycleTimer = std::chrono::high_resolution_clock::now();
    elapsedTime = cycleTimer - cycleTimer;
}

void Timer::update() {
    elapsedTime += std::chrono::high_resolution_clock::now() - cycleTimer;
    cycleTimer = std::chrono::high_resolution_clock::now();
}

bool Timer::loop() {
    return elapsedTime > cycleDuration;
}

void Timer::cycle() {
    elapsedTime -= cycleDuration;
}



