#ifndef _MAIN_
#define _MAIN_

#include <stdio.h>
#include <iostream>
#include "graphics.h"


#define SCREENWIDTH  640
#define SCREENHEIGHT 480
#define NUMSPRITES   32
#define TEXWIDTH     64
#define TEXHEIGHT    64
#define MAPSIZE      6


void init();


/* loads all textures */
/* loads world (calls generator, initializes player position)*/
void loadWorld();

void draw();


void update();

/* gets logic*/
void logic();

/* gets input*/
void input();


#endif // _MAIN_
