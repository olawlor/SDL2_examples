/* 
Simple Directmedia Library (SDL2) Texture from Memory Example

This example draws the runtime stack (!) using SDL2.
Calling functions changes the stack binary data, which looks neat.

SDL example originally by Andrew Baluyot (2024-11-15)
Sprites added by Orion Lawlor, lawlor@alaska.edu, 2025-10-09 (Public Domain)
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

// Globals for SDL drawing
SDL_Window *window=0;
SDL_Renderer *renderer=0;

// Error exit function
void fail(const char *why) {
    printf("--\n");
    printf("Fatal error: %s\n",why);
    printf("Last SDL error: %s\n",SDL_GetError());
    exit(1);
}

// Event handling, see https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlevent.html

void *compiler_deoptimizer=0; // <- keeps compiler from optimizing stuff away

// Write a bunch of stack space
void doStackW()
{
    const int n=50000;
    char buf[n];
    for (int i=0;i<n;i++) buf[i]=0x80; // gray
    compiler_deoptimizer = &buf;
    printf("Uploaded some stack space at %p\n",compiler_deoptimizer);
}

// Recursive call

// Fibonacci has low total stack depth
int recfib(int i) {
    if (i<2) return 1;
    else return recfib(i-1) + recfib(i-2);
}

// Recursive decrement has high total stack depth
int recdec(int i) {
    if (i<2) return 1;
    else return recdec(i-1) + 1;
}

void doStackR() {
    //recfib(10);
    recdec(1000);
}


void doKey(SDL_KeyboardEvent *event, int down) {
    // Handle keyboard event here!
    if (down && event->keysym.sym==SDLK_w) { // write a bunch of stack space
        doStackW();
    }
    if (down && event->keysym.sym==SDLK_r) { // recurse through a bunch of stack space
        doStackR();
    }
    if (down && event->keysym.sym==SDLK_SPACE) { // spacebar quits
        exit(0);
    }
}

void doMouse(SDL_MouseMotionEvent *event) {
    // Handle mouse event here!
}

void handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                exit(0);
                break;

            case SDL_MOUSEMOTION:
                doMouse(&event.motion);
                break;
                
            case SDL_KEYDOWN:
                doKey(&event.key,1);
                break;

            case SDL_KEYUP:
                doKey(&event.key,0);
                break;

            default:
                break;
        }
    }
}

/// Draw this texture onscreen, starting at this corner location (x,y), and size (w,h)
void drawTexture(SDL_Texture *texture,int x,int y, int w,int h)
{
    SDL_Rect dest;
    dest.x=x; dest.y=y;
    dest.w=w; dest.h=h;
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}



int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) fail("init SDL");
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG); // load PNG or JPEG images
    
    window = SDL_CreateWindow("Stack Memory Display", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 512, 0);
    if (!window) fail("Create window");
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) fail("Create renderer");
    
    
    // Size of memory area to display
    int w=256, h=128;
    
    // Zoom factor during render
    int zoom=4;
    
    // See: https://wiki.libsdl.org/SDL2/SDL_CreateTexture
    // See: https://wiki.libsdl.org/SDL2/SDL_PixelFormatEnum
    SDL_Texture *tex = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        w,h);
    if (!tex) fail("CreateTexture");
    
    while (1) { // main loop of game
        handleInput();
        
        SDL_RenderClear(renderer);
        
        SDL_Rect rect;
        rect.x=0; rect.y=0;
        rect.w=w; rect.h=h;
        int pitch=w*3;
        const void *pixels=
            //(const void *)&SDL_CreateTexture; // library's machine code
            (((const char *)&rect)-3*w*h); // the stack (live!)
        
        // See https://wiki.libsdl.org/SDL2/SDL_UpdateTexture
        if (SDL_UpdateTexture(tex,&rect,pixels,pitch)) fail("UpdateTexture");
        
        drawTexture(tex,0,0,w*zoom,h*zoom);
        
        
        SDL_RenderPresent(renderer); // show rendered stuff
        
        SDL_Delay(10); // caps framerate at up to 100Hz
    }
    
    return 0;
}
