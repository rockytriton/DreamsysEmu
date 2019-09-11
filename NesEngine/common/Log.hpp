//
//  Log.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include "common.h"

#include <fstream>
#include <iostream>
#include <iomanip>

using std::endl;

namespace nes {
    class Log {
    public:
        std::ostream &stream() {
            return *outStream;
        }
        
        static Log &inst() {
            static Log INST;
            return INST;
        };
        
        bool enabled() {
            return true;
        }
        
    private:
        Log();
        ~Log() {}
        
        std::ostream *outStream = &std::cout;
    };
    
    static std::ostream& operator <<(std::ostream &os, Byte b)
    {
        os << (unsigned)b;
        return os;
    }
    
    static std::ostream& operator <<(std::ostream &os, const string &str)
    {
        os << str.c_str();
        return os;
    }
    
    static std::ostream &hex8(std::ostream &os) {
        os << std::setfill('0') << std::setw(2) << std::hex;
        return os;
    }
    
    static std::ostream &hex16(std::ostream &os) {
        os << std::setfill('0') << std::setw(4) << std::hex;
        return os;
    }
    
    static std::ostream &hex32(std::ostream &os) {
        os << std::setfill('0') << std::setw(8) << std::hex;
        return os;
    }
    
    static std::ostream &hex64(std::ostream &os) {
        os << std::setfill('0') << std::setw(16) << std::hex;
        return os;
    }
    
    #define LOG Log::inst().stream()
    #define LOG_ENABLED Log::inst().enabled()
    
}


#endif /* Log_hpp */
