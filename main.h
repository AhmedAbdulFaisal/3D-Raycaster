#include <stdio.h>
#include <iostream>
#include <vector>
#include "graphics.h"
//originally 6

/**
 * Rendering caveats:
 *  320x200 (DOS resolution) seems to render in the center within a border due to the monitor not supporting 320x200 CGA. This is expected
 * as the monitor of the dev machine does not support 320x200.
 * Game is expected to run at 640x480 but it looks nicer at 320x200. Possibly introduce some sort of double scaling algorithim? 
 * 896 x 504
 */

#define mapWidth     24
#define mapHeight    24
#define texWidth     64
#define texHeight    64
#define screenWidth  640
#define screenHeight 480
#define FULLSCREEN   1
#define numSprites   32




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
#define ROCK            "art/textures/rock.png" 
#define GROUND          "art/textures/ground01.png" 
#define COBBLE02        "art/textures/cobble002.png"
#define ROCKWEAK        "art/textures/rockweak.png"



/* SPRITES DEFINES */

#define BARREL          "art/sprites/barrel.png"
#define PILLAR          "art/sprites/pillar.png"
#define PILLAR001          "art/sprites/pillar001.png"
#define STAL001         "art/sprites/stal001.png"
#define STAL002         "art/sprites/stal002.png"
#define STAL003         "art/sprites/stal003.png"




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
void exploreMap(int map[], int radius, int px, int py);

void playerInit();
int isInRadius(int x, int y, int px,int py, int radius);

void renderWall();
void renderTile();
void renderSprite();

