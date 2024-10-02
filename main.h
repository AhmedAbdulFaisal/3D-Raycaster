#ifndef _MAIN_
#define _MAIN_

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

void playerInit();

void renderScene();

#endif //_LOAD_
