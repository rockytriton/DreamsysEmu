//
//  Timer.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Timer_hpp
#define Timer_hpp

#include <chrono>

namespace nes::system {
    
    class Timer {
    public:
        Timer();
        void update();
        bool loop();
        void cycle();
        
    private:
        std::chrono::high_resolution_clock::duration elapsedTime;
        std::chrono::nanoseconds cycleDuration = std::chrono::nanoseconds(590);
        std::chrono::high_resolution_clock::time_point cycleTimer;
        
    };
}

#endif /* Timer_hpp */
