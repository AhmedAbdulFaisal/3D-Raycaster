#include "main.h"
#include "entity.h"
#include "renderer.h"
#include <SDL/SDL.h>
#include "utility.h"
#include <vector>
#include <math.h>
#include <stdio.h>
#include "load.h"

using namespace QuickCG;
using namespace std;

/**
 * So far, this remake has been quite broken. Putting the renderer in a seperate class
 * just flat out does not work, I will probably update this later with SDL 2.0 and 
 * work on my original prototype 
 */

/* data structures */
struct Player p;
struct World wld;

/* time keeping variables */
double t;
double oldt;

Renderer ren;

int spriteOrder[NUMSPRITES];
double spriteDistance[NUMSPRITES];


/* for loading assets  */
std::vector<Uint32> texture[256]; //texture array
std::vector<Uint32> sprites[10]; //sprite array

void init()
{

    /* world initialization */

    p.dirX = -1;
    p.dirY = 0;
    p.planeX = 0;
    p.planeY = 0.66;

    p.xPos = 2;
    p.yPos = 2;

    wld = {6,
           {1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1},
           {1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 1,
            1, 0, 0, 0, 0, 1,
            1, 1, 0, 1, 0, 1,
            1, 0, 0, 0, 0, 1,
            1, 1, 1, 1, 1, 1},
           {1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1}};

    printf("Array Size: %d\n", wld.size);

    printArray(wld.ceiling, 6);
    printArray(wld.walls, 6);
    printArray(wld.floor, 6);

    ren.set(p,wld);

    t = 0;
    oldt = 0;

    initTextures(texture,sprites);

    screen(SCREENWIDTH, SCREENHEIGHT, 0, "Raycaster Engine 2");

    loadTextures(texture,sprites);
}

int main(int argc, char *argv[])
{
    init();

    while (!done())
    {
        draw();
        update();
    }
}

/* put rendering logic here */
void draw()
{
    ren.drawScene(texture);
    ren.clScreen();
    ren.drawHud();

    QuickCG::redraw();
    
}

/* put game logic here */
void update()
{

    oldt = t;
    t = getTicks();

    double frameTime = (t - oldt) / 1000.0;

    ren.set(p,wld);
    p.moveSpeed = frameTime * 3.0;
    p.rotSpeed = frameTime * 2.0;

    input();
}

void input()
{
    QuickCG::readKeys();

    // move forward if no wall in front of you
    if (keyDown(SDLK_UP))
    {
        /* arr[size * x + y]*/
        if (wld.walls[int(p.xPos + p.dirX * p.moveSpeed) * wld.size + int(p.yPos)] == false)
            p.xPos += p.dirX * p.moveSpeed;
        if (wld.walls[int(p.xPos) * wld.size + int(p.yPos + p.dirY * p.moveSpeed)] == false)
            p.yPos += p.dirY * p.moveSpeed;
    }
    // move backwards if no wall behind you
    if (keyDown(SDLK_DOWN))
    {
        if (wld.walls[int(p.xPos - p.dirX * p.moveSpeed) * wld.size + int(p.yPos)] == false)
            p.xPos -= p.dirX * p.moveSpeed;
        if (wld.walls[int(p.xPos) * wld.size + int(p.yPos - p.dirY * p.moveSpeed)] == false)
            p.yPos -= p.dirY * p.moveSpeed;
    }
    // rotate to the right
    if (keyDown(SDLK_RIGHT))
    {
        // both camera direction and camera plane must be rotated
        double oldDirX = p.dirX;
        p.dirX = p.dirX * cos(-p.rotSpeed) - p.dirY * sin(-p.rotSpeed);
        p.dirY = oldDirX * sin(-p.rotSpeed) + p.dirY * cos(-p.rotSpeed);
        double oldPlaneX = p.planeX;
        p.planeX = p.planeX * cos(-p.rotSpeed) - p.planeY * sin(-p.rotSpeed);
        p.planeY = oldPlaneX * sin(-p.rotSpeed) + p.planeY * cos(-p.rotSpeed);
    }
    // rotate to the left
    if (keyDown(SDLK_LEFT))
    {
        // both camera direction and camera plane must be rotated
        double oldDirX = p.dirX;
        p.dirX = p.dirX * cos(p.rotSpeed) - p.dirY * sin(p.rotSpeed);
        p.dirY = oldDirX * sin(p.rotSpeed) + p.dirY * cos(p.rotSpeed);
        double oldPlaneX = p.planeX;
        p.planeX = p.planeX * cos(p.rotSpeed) - p.planeY * sin(p.rotSpeed);
        p.planeY = oldPlaneX * sin(p.rotSpeed) + p.planeY * cos(p.rotSpeed);
    }
}