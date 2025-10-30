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
int firing = 0;
unsigned int firetime = 0; // framecount when we last spawned bullet

// Array of enemies
const int maxEnemy=500;
int enemyX[maxEnemy], enemyY[maxEnemy];
int enemyH[maxEnemy]; // health, 0 to 100

// Array of bullets
const int maxBullet=30;
int bulletX[maxBullet], bulletY[maxBullet];
int bulletH[maxBullet]; // health, 0 to 100



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
    
    if (event->state != 0) { // mouse is down, fire!
        firing=1;
    } else {
        firing=0;
    }
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

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                {
                    SDL_MouseMotionEvent e;
                    e.type=event.button.type;
                    e.state=event.button.state;
                    e.x=event.button.x;
                    e.y=event.button.y;
                    e.xrel = e.yrel = 0;
                    doMouse(&e);
                }
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


// Textures, loaded in main
SDL_Texture *playerTexture=0;


SDL_Texture *lallaTexture=0;
SDL_Texture *laserTexture=0;

void drawLalla(int x,int y, int with_laser=0, int size=64)
{
    drawTexture(lallaTexture,x,y,size,size);
    if (with_laser) 
        drawTexture(laserTexture,x,y,size,size);
}


SDL_Texture *bulletTexture=0;
void drawBullet(int x,int y)
{
    drawTexture(bulletTexture,x,y,32,64);
}


// Run game physics and draw updated game world
void drawGame()
{
    const unsigned int frames_per_bullet = 10;
    if (firing && framecount-firetime>frames_per_bullet) 
    { // fire another bullet
        firetime=framecount;
    
        // Spawn a new bullet at the player location
        for (int b=0;b<maxBullet;b++) if (bulletH[b]<=0) {
            bulletX[b]=playaX-18; // line up with finger
            bulletY[b]=playaY+12;
            bulletH[b]=100;
            break; //<- we spawned a bullet, don't spam them all
        }
    }

    
    drawTexture(playerTexture,playaX,playaY,128,128);
    
    // Draw enemies
    for (int e=0;e<maxEnemy;e++) {
        drawLalla(enemyX[e],enemyY[e],enemyH[e]<80);
        
        if (enemyH[e]<=0) { // respawn above top
            enemyX[e]=rand()%1024;
            enemyY[e]=(rand()%128)-200;
            enemyH[e]=100;
        }
        
        // Slowly descend, pixel by pixel
        enemyY[e]+= (rand()%2);
        
        // Drift unpredictably
        enemyX[e]+= (rand()%3)-1;
        
        // Bullet interaction
        for (int b=0;b<maxBullet;b++) if (bulletH[b]>0) 
        {
            int dx=bulletX[b]-enemyX[e];
            int dy=bulletY[b]-enemyY[e];
            int len2 = dx*dx + dy*dy; // square of enemy-bullet distance
            int hitlen2 = 30*30; // square of interaction length
            if (len2<hitlen2) { // enemy hit by bullet
                enemyH[e]-=10; // small damage, but repeats per frame
                bulletH[b]-=20; // bullets take damage too
            }
        }
    }
    
    // Draw bullets
    for (int b=0;b<maxBullet;b++) if (bulletH[b]>0) 
    {
        drawBullet(bulletX[b],bulletY[b]);
        
        if (bulletY[b]<=-100) { // above top, despawn
            bulletH[b]=0;
        }
        
        // Rise quickly
        bulletY[b]-=5;
    }
}


int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) fail("init SDL");
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG); // load PNG or JPEG images
    
    window = SDL_CreateWindow("Sprite-based Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 700, 0);
    if (!window) fail("Create window");
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) fail("Create renderer");
    
    
    playerTexture = IMG_LoadTexture(renderer, "playa.png");
    if (!playerTexture) fail("Create playa texture");
    
    lallaTexture = IMG_LoadTexture(renderer, "lalla.png");
    if (!lallaTexture) fail("Create lalla texture");
    
    laserTexture = IMG_LoadTexture(renderer, "laser.png");
    if (!laserTexture) fail("Create laser texture");
    
    bulletTexture = IMG_LoadTexture(renderer, "bullet.png");
    if (!bulletTexture) fail("Create bullet texture");
    
    // Make random enemies
    for (int e=0;e<maxEnemy;e++) {
        enemyX[e]=rand()%1024;
        enemyY[e]=rand()%128;
        enemyH[e]=100;
    }
    // No active bullets
    for (int b=0;b<maxBullet;b++) {
        bulletH[b]=0;
    }
    // Startup test bullet
    bulletX[0]=200;
    bulletY[0]=600;
    bulletH[0]=100;
    
    while (1) { // main loop of game
        handleInput();
        
        SDL_RenderClear(renderer);
        
        drawGame();
        
        SDL_RenderPresent(renderer); // show rendered stuff
        framecount++;
        
        SDL_Delay(10); // caps framerate at up to 100Hz
    }
    
    return 0;
}
