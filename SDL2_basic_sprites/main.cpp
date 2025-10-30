/* 
Simple Directmedia Library (SDL2) Sprite Graphics Example


SDL example originally by Andrew Baluyot (2024-11-15)
Sprites added by Orion Lawlor, lawlor@alaska.edu, 2025-10-03
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

// Globals for SDL drawing
SDL_Window *window=0;
SDL_Renderer *renderer=0;

// Frame counter, basic notion of time
unsigned int framecount=0;

// Player location
int playaX=300, playaY=600;


// Error exit function
void fail(const char *why) {
    printf("--\n");
    printf("Fatal error: %s\n",why);
    printf("Last SDL error: %s\n",SDL_GetError());
    exit(1);
}

// Event handling, see https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlevent.html

void doKey(SDL_KeyboardEvent *event, int down) {
    // Handle keyboard event here!
    if (down && event->keysym.sym==SDLK_SPACE) { // spacebar quits
        exit(0);
    }
}

void doMouse(SDL_MouseMotionEvent *event) {
    // Handle mouse event here!
    playaX = event->x;
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

/// Draw this texture onscreen, starting at this center location (x,y), and size (w,h)
void drawTexture(SDL_Texture *texture,int x,int y, int w,int h)
{
    SDL_Rect dest;
    dest.x=x-w/2; dest.y=y-h/2;
    dest.w=w; dest.h=h;
    SDL_RenderCopy(renderer, texture, NULL, &dest);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) fail("init SDL");
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG); // load PNG or JPEG images
    
    window = SDL_CreateWindow("Sprite Motion", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 700, 0);
    if (!window) fail("Create window");
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) fail("Create renderer");
    
    
    SDL_Texture *playerTexture = IMG_LoadTexture(renderer, "playa.png");
    if (!playerTexture) fail("Create playa texture");
    
    
    while (1) { // main loop of game
        handleInput();
        
        SDL_RenderClear(renderer);
        
        drawTexture(playerTexture,playaX,playaY,128,128);
        
        SDL_RenderPresent(renderer); // show rendered stuff
        framecount++;
        
        SDL_Delay(10); // caps framerate at up to 100Hz
    }
    
    return 0;
}
