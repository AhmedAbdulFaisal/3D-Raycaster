#ifndef _LOAD_
#define _LOAD_

#include "entity.h"
#include <stdint.h>
#include "main.h"
#include "graphics.h"


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

using namespace QuickCG;

void initTextures(std::vector<Uint32> texture[256], std::vector<Uint32> sprites[10]);
void loadTextures(std::vector<Uint32> texture[256], std::vector<Uint32> sprites[10]);


#endif // _LOAD_