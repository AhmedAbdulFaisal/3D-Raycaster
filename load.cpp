#include "load.h"


void initTextures(std::vector<Uint32> texture[256], std::vector<Uint32> sprites[10]) {
    for (int i = 0; i < 13; i++)
        texture[i].resize(texWidth * texHeight);

    for (int i = 0; i < 7; i++)
        sprites[i].resize(texWidth * texHeight);
}
void loadTextures(std::vector<Uint32> texture[256], std::vector<Uint32> sprites[10])
{
    unsigned long tw, th, error = 0;
    error |= loadImage(texture[0], tw, th, BRICKGLARGE);
    error |= loadImage(texture[1], tw, th, BRICKOLARGE);
    error |= loadImage(texture[2], tw, th, BRICKO);
    error |= loadImage(texture[3], tw, th, BRICKG);
    error |= loadImage(texture[4], tw, th, COLUMNDESIGN001);
    error |= loadImage(texture[5], tw, th, COLUMNORANGE);
    error |= loadImage(texture[6], tw, th, DESIGN001);
    error |= loadImage(texture[7], tw, th, DESIGN002);
    error |= loadImage(texture[8], tw, th, COBBLE);
    error |= loadImage(texture[9], tw, th, ROCK);
    error |= loadImage(texture[10], tw, th, GROUND);
    error |= loadImage(texture[11], tw, th, COBBLE02);
    error |= loadImage(texture[12], tw, th, ROCKWEAK);

    /* IMAGE LOADERS - here we load each sprite and add it to it's array */
    error |= loadImage(sprites[1], tw, th, PILLAR);
    error |= loadImage(sprites[2], tw, th, BARREL);
    error |= loadImage(sprites[3], tw, th, STAL001);
    error |= loadImage(sprites[4], tw, th, STAL002);
    error |= loadImage(sprites[5], tw, th, STAL003);
    error |= loadImage(sprites[6], tw, th, PILLAR001);

    /* Error Checkers  */
    if (error)
    {
        printf("ERROR LOADING IMAGES\n");
    }
}