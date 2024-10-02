#ifndef _ENTITY_
#define _ENTITY_

struct World {
    int size=6;
    int ceiling[6*6];
    int walls[6*6];
    int floor[6*6];
};

struct Player {
  double xPos = 0, yPos = 0;
  double dirX = -1, dirY = 0;
  double planeX = 0, planeY = 0.66;
  double moveSpeed = 0;
  double rotSpeed = 0;
};

struct Sprite
{
  double x;
  double y;
  int texture;
};

/** take data from drawScene within renderer.cpp and use it for 
 * game logic
 *  */
struct data {
  double wallPosX;
  double wallPosY;
  double WallDist;
};




#endif // _ENTITY_