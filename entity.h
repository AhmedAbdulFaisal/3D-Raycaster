#include <stdio.h>
#include <iostream>
#include <vector>

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

