//
//  Filters.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/10/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Filters_hpp
#define Filters_hpp

#include "common.h"

namespace nes::apu {
    class Filter {
    public:
        Filter(uint32_t f, uint32_t sampleRate) : RC { 1.0 / (2 * 3.1415928535 * double(f)) }, dt { 1.0 / double(sampleRate) } {}
        virtual ~Filter() = default;
        virtual double process(double sample) = 0;
    
    protected:
        double RC;
        double dt;
    };
    
    class LoPassFilter : public Filter {
    public:
        LoPassFilter(uint32_t f, uint32_t sampleRate) : Filter(f, sampleRate) {
            a = dt / (RC + dt);
            prevX = 0;
            prevY = 0;
        }
        
        virtual ~LoPassFilter() = default;
        
        virtual double process(double x) {
            double y = a * x + (1 - a) * prevY;
            prevX = x;
            prevY = y;
            
            return y;
        }
    private:
        double a;
        double prevX;
        double prevY;
    };
    
    class HiPassFilter : public Filter {
    public:
        HiPassFilter(uint32_t f, uint32_t sampleRate) : Filter(f, sampleRate) {
            a = RC / (RC + dt);
            prevX = 0;
            prevY = 0;
        }
        
        virtual ~HiPassFilter() = default;
        
        virtual double process(double x) {
            double y = a * prevY + a * (x - prevX);
            prevX = x;
            prevY = y;
            
            return y;
        }
    private:
        double a;
        double prevX;
        double prevY;
    };
}

#endif /* Filters_hpp */
