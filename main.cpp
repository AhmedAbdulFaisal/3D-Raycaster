#include <stdio.h>
#include <iostream>
#include <vector>
#include <SDL/SDL.h>
#include "graphics.h"
#include "main.h"
#include "effects.h"
#include <math.h>
#include "generator.h"

using namespace QuickCG;
using namespace std;



/**
 * 
 * RATCASTING DUNGEON CRAWLER
 * (By Ahmed Faisal)
 * 
 * This is a small project of a raycasting nethack-style dungeon crawler where the player must explore each level 
 * before dying. Heavily adapted from https://lodev.org/cgtutor/raycasting.html, and then modified.
 * 
 * TODO: re-write some of graphics.cpp and graphics.h to be more c-friendly (remove namespaces, extraneous functions)
 * TODO: optimize the rendering to be less taxing on the CPU (possibly make it GPU-based)
 * TODO: implement level structure
 * 
 * FLAGS: -g for debugger
 * 
 */

int gen[genWidth][genHeight] = {{ 0 }};
int buf[genWidth][genHeight] = {{ 0 }};


int walls[mapWidth][mapHeight] = {{ 0 }};
int floorTiles[mapWidth][mapHeight] = {{ 0 }};
int ceilTiles[mapWidth][mapHeight] = {{ 0 }};
/**
int walls[mapWidth][mapHeight] =
    {
        {4, 4, 4, 4, 4, 4},
        {4, 0, 0, 0, 0, 4},
        {4, 0, 0, 0, 0, 4},
        {4, 0, 0, 0, 0, 4},
        {4, 0, 0, 0, 0, 4},
        {4, 4, 4, 4, 4, 4}};

int floorTiles[mapWidth][mapHeight] = {
    {3, 3, 3, 3, 3, 3},
    {3, 8, 8, 8, 8, 3},
    {3, 8, 7, 7, 8, 3},
    {3, 8, 7, 7, 8, 3},
    {3, 8, 8, 8, 8, 3},
    {3, 3, 3, 3, 3, 3}};

int ceilTiles[mapWidth][mapHeight] = {
    {3, 3, 3, 3, 3, 3},
    {3, 8, 8, 8, 8, 3},
    {3, 8, 8, 8, 8, 3},
    {3, 8, 8, 8, 8, 3},
    {3, 8, 8, 8, 8, 3},
    {3, 3, 3, 3, 3, 3}};

*/

Sprite sprite[numSprites] =
    {
        // some columns around the map
        {1.5, 1.5, 1},
        {2.5, 1.5, 1},
        {3.5, 1.5, 1},
};

Uint32 buffer[screenHeight][screenWidth];
double Zbuffer[screenWidth];

int spriteOrder[numSprites];
double spriteDistance[numSprites];
world level = {walls, mapWidth, mapHeight};

/* TODO:
 * Fog calculations are way too resource intensive, we are loosing 10 fps on this, need faster way to calculate
 * Things that don't work: converting uint32 to ints, Using different method for color calculations
 * Things that may work: using openGL
 * Things that did kinda work: inline functions, still very slow with 2D billboard sprites.
 **/

int main(int argc, char *argv[])
{
    //printf("here");
    CellularAutomataGenerate(gen,buf,0.5f,2);
    copyMap(gen,ceilTiles,walls,floorTiles);
    printMap(walls);
    //printArray(ceilTiles);
    //printArray(walls);
    //printArray(floorTiles);

    /* PLAYER DATA */
    double posX = 3, posY = 3;


    //rudimentary player positioning system
    for (int x = 0; x < genWidth; x++) {
        for (int y = 0; y < genHeight; y++) {
            if (walls[x][y] == 0) {
                posX = x + .5;
                posY = y + .5;
                break;           
            }
        }
    }

    if ((posX >= mapWidth) || (posY >= mapHeight))
    {
        printf("Error: Player is out of map bounds! Exiting...");
        return 0;
    }



    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66;

    /* TIME KEEPERS */
    double time = 0;
    double oldTime = 0;
    std::vector<Uint32> texture[9];
    std::vector<Uint32> sprites[9];

    /* RESIZE TEXTURE VECTOR */

    for (int i = 0; i < 8; i++)
        texture[i].resize(texWidth * texHeight);

    /* INITIALIZE SCREEN */
    screen(screenWidth, screenHeight, 0, "Raycaster Official");

    /* TEXTURE LOADERS */

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

    /* IMAGE LOADERS */
    error |= loadImage(sprites[1], tw, th, PILLAR);
    error |= loadImage(sprites[2], tw, th, BARREL);

    if (error)
    {
        std::cout << "error loading images" << std::endl;
        return 1;
    }

    /* Load fog variables */

    ColorRGB fog_rgb = INTtoRGB(FOG_COLOR);

    /* LOAD MAP */

    /* Main game loop */
    while (!done())
    {
        /* FLOOR CASTING */
        /* 
        for (int y = 0; y < h; y++)
        {

            

            float rayDirX0 = dirX - planeX;
            float rayDirY0 = dirY - planeY;
            float rayDirX1 = dirX + planeX;
            float rayDirY1 = dirY + planeY;

            int p = y - screenHeight / 2;
            float posZ = 0.5 * screenHeight;

            float rowDistance = posZ / p;

            float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
            float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

            float floorX = posX + rowDistance * rayDirX0;
            float floorY = posY + rowDistance * rayDirY0;

            int cx = (int)(floorX);
            int cy = (int)(floorY);

            printf("%d,%d ", cx, cy);

            float fogPercentage = rowDistance / FOG_DISTANCE;

            if (fogPercentage > 1.0f)
                fogPercentage = 1.0f;

            for (int x = 0; x < screenWidth; ++x)
            {
                int cellX = (int)(floorX);
                int cellY = (int)(floorY);

                int tx = (int)(texWidth * (floorX - cellX)) & (texWidth - 1);
                int ty = (int)(texHeight * (floorY - cellY)) & (texHeight - 1);

                floorX += floorStepX;
                floorY += floorStepY;

                // int texNum_floor = floorTiles[cellX][cellY] - 1;
                // int texNum_ceil = ceilTiles[cellX][cellY] - 1;

                int floorTexture = 3;
                int ceilingTexture = 6;
                // int floorTexture = texNum_floor;
                // int ceilingTexture = texNum_ceil;

                Uint32 color;
                color = texture[floorTexture][texWidth * ty + tx];

                // color = compute_fog_banded(fogPercentage, color);
                color = compute_fog(fogPercentage, color, fog_rgb);
                color = (color >> 1) & 8355711;
                buffer[y][x] = color;

                color = texture[ceilingTexture][texWidth * ty + tx];

                // color = compute_fog_banded(fogPercentage, color);
                color = compute_fog(fogPercentage, color, fog_rgb);
                color = (color >> 1) & 8355711;
                buffer[screenHeight - y - 1][x] = color;
            }
            
        }
        
        */

        /* WALL CASTING */
        for (int x = 0; x < w; x++)
        {
            double cameraX = 2 * x / double(w) - 1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = int(posX);
            int mapY = int(posY);

            double sideDistX;
            double sideDistY;

            double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

            double perpWallDist;

            int stepX;
            int stepY;

            int hit = 0;
            int side;

            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }
            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }

            while (hit == 0)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                /**
                if (tiles[mapX][mapY].isHollow = 0) {
                    hit = 1;
                }
                */

                if (walls[mapX][mapY] > 0)
                {
                    hit = 1;
                }
            }

            // fisheye fix
            if (side == 0)
                perpWallDist = (sideDistX - deltaDistX);
            else
                perpWallDist = (sideDistY - deltaDistY);

            int lineHeight = (int)(h / perpWallDist);
            int drawStart = -lineHeight / 2 + h / 2;
            if (drawStart < 0)
                drawStart = 0;
            int drawEnd = lineHeight / 2 + h / 2;
            if (drawEnd >= h)
                drawEnd = h - 1;

            /* TEXTURE RENDERING CODE */
            int texNum = walls[mapX][mapY] - 1;
            // int texNum = tiles[mapX][mapY].wall;
            // printf("%d",texNum);
            double wallX;
            if (side == 0)
                wallX = posY + perpWallDist * rayDirY;
            else
                wallX = posX + perpWallDist * rayDirX;
            wallX -= floor((wallX));

            int texX = int(wallX * double(texWidth));
            if (side == 0 && rayDirX > 0)
                texX = texWidth - texX - 1;
            if (side == 1 && rayDirY < 0)
                texX = texWidth - texX - 1;

            double step = 1.0 * texHeight / lineHeight;
            double texPos = (drawStart - h / 2 + lineHeight / 2) * step;

            double fogPercentage = perpWallDist / FOG_DISTANCE;

            if (fogPercentage > 1.0f)
                fogPercentage = 1.0f;

            for (int y = drawStart; y < drawEnd; y++)
            {
                // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
                int texY = (int)texPos & (texHeight - 1);
                texPos += step;
                Uint32 color = texture[texNum][texHeight * texY + texX];
                // make color darker for y-sides: R, G and B byte each divided through two with a "shift" and an "and"

                // Uint32 oldcolor = color;

                // color = compute_fog_banded(fogPercentage, color);
                color = compute_fog(fogPercentage, color, fog_rgb);

                if (side == 1)
                    color = (color >> 1) & 8355711;

                buffer[y][x] = color;
            }

            // gay code

            double floorXWall, floorYWall;

            if (side == 0 && rayDirX > 0)
            {
                floorXWall = mapX;
                floorYWall = mapY + wallX;
            }
            else if (side == 0 && rayDirX < 0)
            {
                floorXWall = mapX + 1.0;
                floorYWall = mapY + wallX;
            }
            else if (side == 1 && rayDirY > 0)
            {
                floorXWall = mapX + wallX;
                floorYWall = mapY;
            }
            else
            {
                floorXWall = mapX + wallX;
                floorYWall = mapY + 1.0;
            }

            

            double distWall, distPlayer, currentDist;
            distWall = perpWallDist;
            distPlayer = 0.0;

            if (drawEnd < 0)
                drawEnd = h;
            

            for (int y = drawEnd + 1; y < h; y++)
            {
                currentDist = h / (2.0 * y - h);
                double weight = (currentDist - distPlayer) / (distWall - distPlayer);
                double currentFloorX = weight * floorXWall + (1.0 - weight) * posX;
                double currentFloorY = weight * floorYWall + (1.0 - weight) * posY;

                int floorTexX, floorTexY;
                floorTexX = int(currentFloorX * texWidth) & (texWidth - 1);
                floorTexY = int(currentFloorY * texHeight) & (texHeight - 1);

                int p = y - screenHeight / 2;
                float posZ = 0.5 * screenHeight;
                float rowDistance = posZ / p;

                fogPercentage = rowDistance / FOG_DISTANCE;

                if (fogPercentage > 1.0f) fogPercentage = 1.0f; //clip to prevent overflow (texture bugs)

                int floorTexture = floorTiles[(int)currentFloorX][(int)currentFloorY];
                int ceilTexture = ceilTiles[(int)currentFloorX][(int)currentFloorY];

                Uint32 color;

                color = texture[floorTexture][texWidth * floorTexY + floorTexX];
                color = compute_fog(fogPercentage, color, fog_rgb);

                //buffer[y][x] = (texture[floorTexture][texWidth * floorTexY + floorTexX] >> 1) & 8355711;
                buffer[y][x] = color;

                color = texture[ceilTexture][texWidth * floorTexY + floorTexX];
                color = compute_fog(fogPercentage, color, fog_rgb);

                //buffer[h-y][x] = (texture[ceilTexture][texWidth * floorTexY + floorTexX] >> 1) & 8355711;
                buffer[h-y][x] = color;
            }

            Zbuffer[x] = perpWallDist;
        }

        for (int i = 0; i < numSprites; i++)
        {
            spriteOrder[i] = i;
            spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)); // sqrt not taken, unneeded
        }
        sortSprites(spriteOrder, spriteDistance, numSprites);

        for (int i = 0; i < numSprites; i++)
        {
            double spriteX = sprite[spriteOrder[i]].x - posX;
            double spriteY = sprite[spriteOrder[i]].y - posY;

            double invDet = 1.0 / (planeX * dirY - dirX * planeY);

            double transformX = invDet * (dirY * spriteX - dirX * spriteY);
            double transformY = invDet * (-planeY * spriteX + planeX * spriteY);

            int spriteScreenX = int((w / 2) * (1 + transformX / transformY));

            int spriteHeight = abs(int(h / (transformY)));
            int drawStartY = -spriteHeight / 2 + h / 2;
            if (drawStartY < 0)
                drawStartY = 0;
            int drawEndY = spriteHeight / 2 + h / 2;
            if (drawEndY >= h)
                drawEndY = h - 1;

            int spriteWidth = abs(int(h / (transformY)));
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            if (drawStartX < 0)
                drawStartX = 0;
            int drawEndX = spriteWidth / 2 + spriteScreenX;
            if (drawEndX >= w)
                drawEndX = w - 1;

            /* 0.3 is some weird magic number that makes the fog effect equal to how it works on walls
             * no doubt caused by the fact that wall fog is calculated by perpDistance while sprite fog
             * is calculated by the direct distance...*/
            double fogPercentage = (spriteDistance[i] / FOG_DISTANCE) * 0.3;

            if (fogPercentage > 1.0f)
                fogPercentage = 1.0f;

            for (int stripe = drawStartX; stripe < drawEndX; stripe++)
            {
                int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;

                if (transformY > 0 && stripe > 0 && stripe < w && transformY < Zbuffer[stripe])
                    for (int y = drawStartY; y < drawEndY; y++)
                    {
                        int d = (y) * 256 - h * 128 + spriteHeight * 128;
                        int texY = ((d * texHeight) / spriteHeight) / 256;
                        Uint32 color = sprites[sprite[spriteOrder[i]].texture][texWidth * texY + texX];
                        color = compute_fog(fogPercentage, color, fog_rgb);

                        if ((color & 0x00FFFFFF) != 0)
                            buffer[y][stripe] = color;
                    }
            }
        }

        QuickCG::drawBuffer(buffer[0]);
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                buffer[y][x] = 0; // clear the buffer instead of cls()

        oldTime = time;
        time = getTicks();

        double frameTime = (time - oldTime) / 1000.0;
        QuickCG::print(1.0 / frameTime);
        QuickCG::redraw();

        double moveSpeed = frameTime * 3.0;
        double rotSpeed = frameTime * 2.0;

        QuickCG::readKeys();
        // move forward if no wall in front of you
        if (keyDown(SDLK_UP))
        {
            /**
            if (tiles[int(posX + dirX * moveSpeed)][int(posY)].isHollow = 1) {
                posX += dirX * moveSpeed;
            }
            if (tiles[int(posX)][int(posY + dirY * moveSpeed)].isHollow = 1) {
                posX += dirY * moveSpeed;
            }
            */

            if (walls[int(posX + dirX * moveSpeed)][int(posY)] == false)
                posX += dirX * moveSpeed;
            if (walls[int(posX)][int(posY + dirY * moveSpeed)] == false)
                posY += dirY * moveSpeed;
        }
        // move backwards if no wall behind you
        if (keyDown(SDLK_DOWN))
        {
            /**
            if (tiles[int(posX + dirX * moveSpeed)][int(posY)].isHollow = 1) {
                posX -= dirX * moveSpeed;
            }
            if (tiles[int(posX)][int(posY + dirY * moveSpeed)].isHollow = 1) {
                posX -= dirY * moveSpeed;
            }
            */

            if (walls[int(posX - dirX * moveSpeed)][int(posY)] == false)
                posX -= dirX * moveSpeed;
            if (walls[int(posX)][int(posY - dirY * moveSpeed)] == false)
                posY -= dirY * moveSpeed;
        }
        // rotate to the right
        if (keyDown(SDLK_RIGHT))
        {
            // both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        // rotate to the left
        if (keyDown(SDLK_LEFT))
        {
            // both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }
    }
}

void sortSprites(int *order, double *dist, int amount)
{
    std::vector<std::pair<double, int>> sprites(amount);
    for (int i = 0; i < amount; i++)
    {
        sprites[i].first = dist[i];
        sprites[i].second = order[i];
    }
    std::sort(sprites.begin(), sprites.end());

    for (int i = 0; i < amount; i++)
    {
        dist[i] = sprites[amount - i - 1].first;
        order[i] = sprites[amount - i - 1].second;
    }
}