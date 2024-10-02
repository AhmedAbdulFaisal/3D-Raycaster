#ifndef _RENDERER_
#define _RENDERER_

#include "entity.h"
#include <stdint.h>
#include "main.h"

class Renderer
{
public:
  std::vector<Sprite> spritelist;
  Sprite sprite[NUMSPRITES];
  Uint32 buffer[SCREENWIDTH][SCREENHEIGHT];
  double Zbuffer[SCREENWIDTH];

  struct Player p;
  struct World wld;

  Renderer() {
    
  }

  void set(struct Player pl, struct World w) {
    p = pl;
    wld = w;
  }

  void drawScene(std::vector<Uint32> texture[256]);
  void drawHud();
  void clScreen();

  void drawMinimap(int map[], int mapScale, struct Player pstruct, int xPos, int yPos);
};

#endif // _RENDERER_