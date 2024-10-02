#ifndef _ENTITY_
#define _ENTITY_

#include <stdio.h>
#include <iostream>
#include <vector>
#include "main.h"

struct Player {
  double posX = 3, posY = 3;
  double dirX = -1, dirY = 0;
  double planeX = 0, planeY = 0.66;
};

struct Sprite
{
  double x;
  double y;
  int texture;
};

struct World {
  int size=mapWidth;
  int ceiling[mapWidth*mapWidth] = {'0'};
  int walls[mapWidth*mapWidth] = {'0'};
  int floor[mapWidth*mapWidth] = {'0'};
};

#endif //_ENTITY_

