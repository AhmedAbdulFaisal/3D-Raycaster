#include <stdio.h>
#include <iostream>
#include <vector>
#include <SDL/SDL.h>
#include "graphics.h"
#include "main.h"
#include "effects.h"
#include <math.h>
#include "generator.h"
#include "load.h"
#include "minimap.h"

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
 * Sys reqs: 4MB of ram (No seriously)
 *
 *
 *
 * TODO: re-write some of graphics.cpp and graphics.h to be less dependent on the source tutorial
 * TODO: optimize the rendering to be less taxing on the CPU (possibly make it GPU-based)
 * TODO: 
 * FLAGS: -g for debugger
 *
 */

/** 
 * gen - array for generation
 * buf - array for holding buffer (for terragen)
 * map - linear array for holding the minimap data
 */
int gen[genWidth*genHeight] = {0};
int buf[genWidth*genHeight] = {{0}};
int map[genWidth*genHeight] = {0};


/* Game structures */
/* TODO: Make class for player, gotta use some of that C++ stuff*/
World wld;
Player p;

//spritelist - for holding all sprites being rendered
//sprite - for holding all possible sprites loaded into the game
std::vector<Sprite> spritelist;
Sprite sprite[numSprites];

//buffer - screen buffer array
//Zbuffer - for holding depth of sprites
Uint32 buffer[screenHeight][screenWidth];
double Zbuffer[screenWidth];

std::vector<Uint32> texture[256]; //texture array
std::vector<Uint32> sprites[10]; //sprite array

//spriteorder - for holding order of sprites
//spriteDistance - array of all possible sprite distances
int spriteOrder[numSprites];
double spriteDistance[numSprites];

//unused for now, structure for holding levels
//world level = {walls, mapWidth, mapHeight};


//universal variables for player position, mostly because they're accessed by an init and main function

int main(int argc, char *argv[])
{

    /* GUI variables and switches */
    int drawMap = 0;
    int mapScale = 4;
    double mapPos = -512;
    int debugScreen = 0;

    /* Store the distance of a wall and the tile position for destroying and creating walls on command */
    double wallPosX = 0, wallPosY = 0;
    double wallDist;


    playerInit(); //call the player init function, to be called for every level restart
    /* variables for direction of player and FOV (0,0.66) */

    /* TIME KEEPERS */
    double time = 0;
    double oldTime = 0;

    /* RESIZE TEXTURE VECTOR - 2d vector which is resized for the size of each texture */
    initTextures(texture, sprites);
    /* INITIALIZE SCREEN */
    screen(screenWidth, screenHeight, 1, "Raycaster Official");

    loadTextures(texture, sprites);

    /* Load fog variable */
    ColorRGB fog_rgb = INTtoRGB(FOG_COLOR);

    /* Main game loop */
    while (!done())
    {

        /* DRAW CODE */
        for (int x = 0; x < w; x++)
        {

            /* WALL CALCULATIONS */
            double cameraX = 2 * x / double(w) - 1;
            double rayDirX = p.dirX + p.planeX * cameraX;
            double rayDirY = p.dirY + p.planeY * cameraX;

            int mapX = int(p.posX);
            int mapY = int(p.posY);

            double sideDistX;
            double sideDistY;

            double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

            double perpWallDist;

            int stepX;
            int stepY;

            int hit = 0;
            int side;

            /* Checking for which side it has hit */
            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (p.posX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - p.posX) * deltaDistX;
            }
            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (p.posY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - p.posY) * deltaDistY;
            }
            /* checking for if it has a hit or side */
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

                if (wld.walls[mapHeight * (mapX) + (mapY)] > 0)
                {
                    hit = 1;
                }
            }

            /* fixes fisheye */
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


            /* TEXTURE RENDERING CODE (WALLS)*/
            int texNum = wld.walls[mapHeight * (mapX) + (mapY)] - 1;
      

            double wallX;
            if (side == 0)
                wallX = p.posY + perpWallDist * rayDirY;
            else
                wallX = p.posX + perpWallDist * rayDirX;
            wallX -= floor((wallX));

            int texX = int(wallX * double(texWidth));
            if (side == 0 && rayDirX > 0)
                texX = texWidth - texX - 1;
            if (side == 1 && rayDirY < 0)
                texX = texWidth - texX - 1;

            /* Take the position of the wall and distance whenever the for loop reaches the middle, that way we can detect exactly what hits in the middle */
            if (x == (w / 2))
            {
                wallPosX = mapX;
                wallPosY = mapY;
                wallDist = perpWallDist;
            }

            /*Make preperations for texture coordinates*/
            double step = 1.0 * texHeight / lineHeight;
            double texPos = (drawStart - h / 2 + lineHeight / 2) * step;

            /* Calculate fog distance based off of perpWallDist */
            double fogPercentage = perpWallDist / FOG_DISTANCE;

            if (fogPercentage > 1.0f)
                fogPercentage = 1.0f;

            /* For every pixel within each vertical stripe */
            for (int y = drawStart; y < drawEnd; y++)
            {
                // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
                int texY = (int)texPos & (texHeight - 1);
                texPos += step;
                Uint32 color = texture[texNum][texHeight * texY + texX];
                

                

                /* Calculate fog (use shader as described in effects.cpp, and make a side darker if it's 1 )*/
                color = compute_fog(fogPercentage, color, fog_rgb);

                if (side == 1)
                    color = (color >> 1) & 8355711;


                /* Save color to buffer */
                buffer[y][x] = color;
            }

            /* FLOOR TEXTURING CODE */

            double floorXWall, floorYWall;

            /* Check which side is which, calculate floorXWall and floorYWall*/
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

            /* Initialize variables */
            double distWall, distPlayer, currentDist;
            distWall = perpWallDist;
            distPlayer = 0.0;

            if (drawEnd < 0)
                drawEnd = h;

            /* Draw a vertical stripe for each floor texture */
            for (int y = drawEnd + 1; y < h; y++)
            {
                /* Calculate texture coordinates and their distance on the Z axis */
                currentDist = h / (2.0 * y - h);
                double weight = (currentDist - distPlayer) / (distWall - distPlayer);
                double currentFloorX = weight * floorXWall + (1.0 - weight) * p.posX;
                double currentFloorY = weight * floorYWall + (1.0 - weight) * p.posY;

                int floorTexX, floorTexY;
                floorTexX = int(currentFloorX * texWidth) & (texWidth - 1);
                floorTexY = int(currentFloorY * texHeight) & (texHeight - 1);

                int p = y - screenHeight / 2;
                float posZ = 0.5 * screenHeight;
                float rowDistance = posZ / p;

                /* Here we calculate the fog percentage in the loop for each stripe */
                fogPercentage = rowDistance / FOG_DISTANCE;

                if (fogPercentage > 1.0f)
                    fogPercentage = 1.0f; // clip to prevent overflow (texture bugs)

                /* Get floor texture and ceiling texture (based off of 2d map )*/
                int floorTexture = wld.floor[mapHeight * ((int)currentFloorX) + ((int)currentFloorY)];
                int ceilTexture = wld.ceiling[mapHeight * ((int)currentFloorX) + ((int)currentFloorY)];

                Uint32 color;

                /* compute fog for both ceiling and floor */
                color = texture[floorTexture][texWidth * floorTexY + floorTexX];
                color = compute_fog(fogPercentage, color, fog_rgb);

                buffer[y][x] = color;

                color = texture[ceilTexture][texWidth * floorTexY + floorTexX];
                color = compute_fog(fogPercentage, color, fog_rgb);

                //for each one, save to color (buffer h-y or y for ceiling and floor)
                buffer[h - y][x] = color;
            }
            //get the perpwalldist and calclate the Zbuffer. 
            Zbuffer[x] = perpWallDist;
        }


        /* Calculate sprite distance */
        for (int i = 0; i < numSprites; i++)
        {
            spriteOrder[i] = i;
            spriteDistance[i] = ((p.posX - sprite[i].x) * (p.posX - sprite[i].x) + (p.posY - sprite[i].y) * (p.posY - sprite[i].y)); // sqrt not taken, unneeded
        }
        
        /* Then, sort the srites given the spriteOrder and spriteDistance (with numSprites)*/
        sortSprites(spriteOrder, spriteDistance, numSprites);

        /* take each sprite (based off the order), and then draw them */
        for (int i = 0; i < numSprites; i++)
        {
            /* define sprite location */
            double spriteX = sprite[spriteOrder[i]].x - p.posX;
            double spriteY = sprite[spriteOrder[i]].y - p.posY;


            
            double invDet = 1.0 / (p.planeX * p.dirY - p.dirX * p.planeY);

            /* factor to transform the sprite */
            double transformX = invDet * (p.dirY * spriteX - p.dirX * spriteY);
            double transformY = invDet * (-p.planeY * spriteX + p.planeX * spriteY);

            int spriteScreenX = int((w / 2) * (1 + transformX / transformY));



            int spriteHeight = abs(int(h / (transformY)));
            int drawStartY = -spriteHeight / 2 + h / 2;
            
            /* clipping parameters */
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

            /* Calculate sprite fog (see magic number explanation below...)*/
            /* 0.3 is some weird magic number that makes the fog effect equal to how it works on walls
             * no doubt caused by the fact that wall fog is calculated by perpDistance while sprite fog
             * is calculated by the direct distance...*/
            double fogPercentage = (spriteDistance[i] / FOG_DISTANCE) * 0.3;

            /* Again, clip fog distance */
            if (fogPercentage > 1.0f)
                fogPercentage = 1.0f;

            /* start with drawStartX and drawEndX, not from 0 to w as that would be the entire screen */
            for (int stripe = drawStartX; stripe < drawEndX; stripe++)
            {
                /* generate x coordinate of texture*/
                int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;

                /* if it's within the transformY bounds, then we draw within the bounds of drawStartY for the y coordinate */
                if (transformY > 0 && stripe > 0 && stripe < w && transformY < Zbuffer[stripe])
                    for (int y = drawStartY; y < drawEndY; y++)
                    {
                        int d = (y) * 256 - h * 128 + spriteHeight * 128;
                        int texY = ((d * texHeight) / spriteHeight) / 256;
                        Uint32 color = sprites[sprite[spriteOrder[i]].texture][texWidth * texY + texX];
                        /* compute fog and if the color is not dark (transparent), we apply the color */
                        color = compute_fog(fogPercentage, color, fog_rgb);

                        if ((color & 0x00FFFFFF) != 0)
                            buffer[y][stripe] = color;
                    }
            }
        }

        /* Draw the buffer, then clear it for other elements */
        QuickCG::drawBuffer(buffer[0]);
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++)
                buffer[y][x] = 0; // clear the buffer instead of cls()


        /* calculate FPS */
        oldTime = time;
        time = getTicks();

        double frameTime = (time - oldTime) / 1000.0;

        /* HUD CODE GOES HERE */

        /* Render map screen (drawMap is not really used for now )*/
        renderMap(map, 10, 32, 32 + mapPos, p.posX, p.posY, p.dirX, p.dirY);
        
        /* Debug Screen parameters */
        if (debugScreen == 1)
        {
            // every 8 pixels
            QuickCG::print("Debug Screen", 0, 0);
            QuickCG::print(1.0 / frameTime, 0, 8);
            QuickCG::print(p.dirX, 0, 16);
            QuickCG::print(p.dirY, 0, 24);
            QuickCG::print(p.posX, 0, 32);
            QuickCG::print(p.posY, 0, 40);
            QuickCG::print(p.posY, 0, 40);
            QuickCG::print(wallPosX, 0, 48);
            QuickCG::print(wallPosY, 0, 56);
            QuickCG::print(wld.walls[mapHeight * ((int)wallPosX) + ((int)wallPosY)], 0, 64);
            QuickCG::print(wallDist, 0, 72);
        }

        /* Now, we re-draw the screen from scratch */
        QuickCG::redraw();

        double moveSpeed = frameTime * 3.0;
        double rotSpeed = frameTime * 2.0;

        /* GAME LOGIC GOES HERE */


        exploreMap(map,wld.walls, 3,p); //code for updating minimap 

        /* INPUT CODE GOES HERE */
        QuickCG::readKeys();

        if (keyDown(SDLK_TAB))
        {
            drawMap = 1;
            mapPos += 25;
            if (mapPos >= 0)
            {
                mapPos = 0;
            }
        }
        else
        {
            mapPos -= 25;
            if (mapPos <= -512)
            {
                mapPos = -512;
            }
            drawMap = 0;
        }

        if (keyPressed(SDLK_e))
        {
            playerInit();
        }

        if (keyPressed(SDLK_SPACE) && (wallDist < 2))
        {

            if ((((int)wallPosX >= 1) && ((int)wallPosX <= mapWidth - 1)) && (((int)wallPosY >= 1) && ((int)wallPosY <= mapHeight - 1)))
            {
                wld.walls[mapHeight * ((int)wallPosX) + ((int)wallPosY)] = 0;
            }
        }

        if (keyDown(SDLK_F3))
        {
            debugScreen = 1;
        }
        else
        {
            debugScreen = 0;
        }

        // move forward if no wall in front of you
        if (keyDown(SDLK_UP))
        {
            if (wld.walls[mapHeight * (int(p.posX + p.dirX * moveSpeed)) + (int(p.posY))] == false)
                p.posX += p.dirX * moveSpeed;
            if (wld.walls[mapHeight * (int(p.posX)) + (int(p.posY + p.dirY * moveSpeed))] == false)
                p.posY += p.dirY * moveSpeed;
        }
        // move backwards if no wall behind you
        if (keyDown(SDLK_DOWN))
        {
            if (wld.walls[mapHeight * (int(p.posX - p.dirX * moveSpeed)) + (int(p.posY))] == false)
                p.posX -= p.dirX * moveSpeed;
            if (wld.walls[mapHeight * (int(p.posX)) + (int(p.posY - p.dirY * moveSpeed))] == false)
                p.posY -= p.dirY * moveSpeed;
        }
        // rotate to the right
        if (keyDown(SDLK_RIGHT))
        {
            // both camera direction and camera plane must be rotated
            double oldDirX = p.dirX;
            p.dirX = p.dirX * cos(-rotSpeed) - p.dirY * sin(-rotSpeed);
            p.dirY = oldDirX * sin(-rotSpeed) + p.dirY * cos(-rotSpeed);
            double oldPlaneX = p.planeX;
            p.planeX = p.planeX * cos(-rotSpeed) - p.planeY * sin(-rotSpeed);
            p.planeY = oldPlaneX * sin(-rotSpeed) + p.planeY * cos(-rotSpeed);
        }
        // rotate to the left
        if (keyDown(SDLK_LEFT))
        {
            // both camera direction and camera plane must be rotated
            double oldDirX = p.dirX;
            p.dirX = p.dirX * cos(rotSpeed) - p.dirY * sin(rotSpeed);
            p.dirY = oldDirX * sin(rotSpeed) + p.dirY * cos(rotSpeed);
            double oldPlaneX = p.planeX;
            p.planeX = p.planeX * cos(rotSpeed) - p.planeY * sin(rotSpeed);
            p.planeY = oldPlaneX * sin(rotSpeed) + p.planeY * cos(rotSpeed);
        }
    }
}

void renderScene() {
    
}


void playerInit()
{

    p.dirX = -1;
    p.dirY = 0;
    p.planeX = 0;
    p.planeY = 0.66;
    /* variations for sprites */
    int variation[] = {3, 4, 5, 6};
    
    /* Generates a new map with cellular automata */
    CellularAutomataGenerate(gen, buf, 0.5f, 1);
    /* Actually copies data over to the mapData arrays*/
    copyMap(gen, wld);

    /* Generates locations of static props */
    generateStaticPositions(wld.walls, sprite, variation);

    // rudimentary player positioning system which finds the first available position
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapWidth; y++) {
            if ((wld.walls[mapHeight * (x) + (y)] == 0)) {
                p.posX = x + 0.5;
                p.posY = y + 0.5;
                
                break;
            }
        }
    }


}

/* Simple sorting mechanism for the sprite ordering code */
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