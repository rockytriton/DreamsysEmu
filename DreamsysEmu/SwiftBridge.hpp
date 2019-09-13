//
//  SwiftBridge.hpp
//  DreamsysEmu
//
//  Created by Rocky Pulley on 9/7/19.
//  Copyright © 2019 Rocky Pulley. All rights reserved.
//

#ifndef SwiftBridge_hpp
#define SwiftBridge_hpp


extern const int BUTTON_A ;
extern const int BUTTON_B ;
extern const int BUTTON_SE;
extern const int BUTTON_ST;
extern const int BUTTON_U ;
extern const int BUTTON_D ;
extern const int BUTTON_L ;
extern const int BUTTON_R ;

#include "common.h"

#ifdef __cplusplus
extern "C" {
#else
    
    struct PixelData **emu_buffer();
    
#endif
    
    int loadCartridge(const char *fileName);
    
    void emu_run();
    
    int emu_running();
    
    unsigned int **emu_image_buffer();
    
    unsigned int emu_current_frame();
    
    void controller_set(int n, unsigned char b, unsigned char state);
    
    int emu_read_audio(float *p);
    
    float *emu_read_samples(unsigned short *n);
    
    void emu_done_read();
    
    void emu_init();
    
    void updateAudio();

    
#ifdef __cplusplus
}
#endif



#endif /* SwiftBridge_hpp */
