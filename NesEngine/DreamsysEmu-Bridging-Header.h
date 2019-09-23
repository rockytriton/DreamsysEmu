//
//  Use this file to import your target's public headers that you would like to expose to Swift.
//

#include "SwiftBridge.hpp"


/*This source code copyrighted by Lazy Foo' Productions (2004-2019)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Size {
    int width;
    int height;
};

class SpriteSheet
{
public:
    SpriteSheet(const char *file, Size size, Size spriteSize) {
        sheet = IMG_Load(file);
        
        if (sheet == nullptr) {
            throw std::string("Failed to load file: ") + std::string(file);
        }
        
        int columns = size.width / spriteSize.width;
        int rows = size.height / spriteSize.height;
        rects = new SDL_Rect *[columns];
        
        for (int y=0; y<size.height; y += spriteSize.height) {
            rects[y / spriteSize.height] = new SDL_Rect[rows];
            
            for (int x=0; x<size.width; x += spriteSize.width) {
                rects[y / spriteSize.height][x / spriteSize.width] = {x, y, spriteSize.width, spriteSize.height};
            }
        }
    }
    
    SDL_Surface *getSheet() { return sheet; }
    
    SDL_Rect *getFrame(int x, int y) { return &rects[y][x]; }
    
private:
    SDL_Surface *sheet = nullptr;
    SDL_Rect **rects;
};

int main( int argc, char* args[] )
{
	//The window we'll be rendering to
	SDL_Window* window = NULL;
	
	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;
    
    int x = 0;
    int y = 0;
    
    int ani = 0;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
        SDL_Init(IMG_INIT_PNG);
        SDL_Surface *sumpf = IMG_Load("/work/tileset_new.png");
        SpriteSheet knight("/work/knightframes.png", {256, 256}, {32, 32});
        //SDL_Surface *knights = IMG_Load("/work/knightframes.png");
        
		//Create window
		window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        SDL_Surface *surface_win = SDL_GetWindowSurface(window);
		if( window == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface( window );

			//Fill the surface white
			//Update the surface

			//Wait two seconds
			SDL_Delay( 2000 );
		}
        bool running = true;
        
        while(running) {
        
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
            
            SDL_BlitSurface(sumpf, NULL, surface_win, NULL);
            
            SDL_Rect rc;
            rc.x = ani * 32;
            rc.y = 3 * 32;
            rc.h = 32;
            rc.w = 32;
            
            ani++;
            
            if (ani > 7) {
                ani = 0;
            }
            
            SDL_Rect dst;
            dst.x = x;
            dst.y = y;
            dst.w = 32;
            dst.h = 32;
            
            x += 3;
            
            SDL_BlitSurface(knight.getSheet(), knight.getFrame(ani, 3), surface_win, &dst);
            //SDL_BlitSurface(knights, &rc, surface_win, &dst);
            SDL_UpdateWindowSurface( window );
            
            SDL_Delay(50);
            
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
            }
            
            
            
            
            if (!running) {
                break;
            }
        }
        }
	}

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}
