//
//  PipelineProcessor.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef PipelineProcessor_hpp
#define PipelineProcessor_hpp

#include "common.h"

namespace nes::cpu {
    class Cpu;
}

namespace nes::ppu {
 
    class Ppu;
    
    class PipelineProcessor {
    public:
        void preRender();
        void render();
        void postRender();
        void vBlank();
        
        void setPpu(Ppu *p) { this->ppu = p; }
        void setCpu(nes::cpu::Cpu *p) { this->cpu = p; }
        
    private:
        void renderVisibleDots();
        
        Ppu *ppu;
        nes::cpu::Cpu *cpu;
    };
    
}

#endif /* PipelineProcessor_hpp */
