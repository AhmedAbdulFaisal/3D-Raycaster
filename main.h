#include <stdio.h>
#include <iostream>
#include <vector>
#include "graphics.h"
//originally 6
#define mapWidth     24
#define mapHeight    24
#define texWidth     64
#define texHeight    64
#define screenWidth  640
#define screenHeight 480
#define FULLSCREEN   1
#define numSprites   19



#define FOG_COLOR    0x010101
#define FOG_DISTANCE 7

/* TEXTURE DEFINES */
#define BRICKGLARGE     "art/textures/brick-large.png"
#define BRICKOLARGE     "art/textures/brickorange-large.png"
#define BRICKG          "art/textures/brick.png"
#define BRICKO          "art/textures/brickorange.png"
#define COBBLE          "art/textures/cobble.png"
#define COLUMNDESIGN001 "art/textures/columndesign001.png"
#define COLUMNORANGE    "art/textures/columnorange.png"
#define DESIGN001       "art/textures/design001.png" 
#define DESIGN002       "art/textures/design002.png" 



/* BARREL DEFINES */

#define BARREL          "art/sprites/barrel.png"
#define PILLAR          "art/sprites/pillar.png"



struct Player {
  double x = 0, y = 0;
  double dirX = -1, dirY = 0;
  double planeX = 0, planeY = 0.66;
};

struct Sprite
{
  double x;
  double y;
  int texture;
};

struct world {
  int (*world)[mapHeight];
  int width, height;
};

/** NOTE: Scrap tile system, too complex for game, ditto with 'lighting system'
 *  TODO: work on new system, here we have a 2d array with numbers, length and width, and some other stuff
 * 
 * 
 **/


void sortSprites(int* order, double* dist, int amount);
void generateMap();
void initTileList();

void renderWall();
void renderTile();
void renderSprite();

