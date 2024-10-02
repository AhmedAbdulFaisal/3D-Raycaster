#ifndef _MINIMAP_
#define _MINIMAP_

#include "entity.h"
#include <stdint.h>
#include "main.h"
#include "graphics.h"

using namespace QuickCG;

void exploreMap(int map[],int walls[], int radius, struct Player p);

void renderMap(int map[], int mapScale, int xPos, int yPos, double px, double py, double rx, double ry);

#endif //_MINIMAP_