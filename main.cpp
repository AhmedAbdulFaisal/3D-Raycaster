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
 * Sys reqs: 4MB of ram (No seriously)
 *
 *
 *
 * TODO: re-write some of graphics.cpp and graphics.h to be less dependent on the source tutorial
 * TODO: optimize the rendering to be less taxing on the CPU (possibly make it GPU-based)
 * TODO: implement level structure
 * TODO:
 * FLAGS: -g for debugger
 *
 */

/** 
 * gen - 2d array for generation
 * buf - 2d array for holding buffer (for terragen)
 * map - linear array for holding the minimap data
 */
int gen[genWidth][genHeight] = {{0}};
int buf[genWidth][genHeight] = {{0}};
int map[genWidth * genHeight] = {0};



//walls, floorTiles, ceilTiles - 2d array for holding the data for ceil, floor, and wall textures 
int walls[mapWidth][mapHeight] = {{0}};
int floorTiles[mapWidth][mapHeight] = {{0}};
int ceilTiles[mapWidth][mapHeight] = {{0}};

//spritelist - for holding all sprites being rendered
//sprite - for holding all possible sprites loaded into the game
std::vector<Sprite> spritelist;
Sprite sprite[numSprites];

//buffer - screen buffer array
//Zbuffer - for holding depth of sprites
Uint32 buffer[screenHeight][screenWidth];
double Zbuffer[screenWidth];

//spriteorder - for holding order of sprites
//spriteDistance - array of all possible sprite distances
int spriteOrder[numSprites];
double spriteDistance[numSprites];

//unused for now, structure for holding levels
world level = {walls, mapWidth, mapHeight};


//universal variables for player position, mostly because they're accessed by an init and main function
double posX = 3, posY = 3;

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
    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66;

    /* TIME KEEPERS */
    double time = 0;
    double oldTime = 0;
    std::vector<Uint32> texture[256]; //texture array
    std::vector<Uint32> sprites[10]; //sprite array

    /* RESIZE TEXTURE VECTOR - 2d vector which is resized for the size of each texture */
    for (int i = 0; i < 256; i++)
        texture[i].resize(texWidth * texHeight);

    /* INITIALIZE SCREEN */
    screen(screenWidth, screenHeight, 0, "Raycaster Official");

    /* TEXTURE LOADERS - here we load each texture and add it to the array*/
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
        std::cout << "error loading images" << std::endl;
        return 1;
    }

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

            /* Checking for which side it has hit */
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

                if (walls[mapX][mapY] > 0)
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
            int texNum = walls[mapX][mapY] - 1;
      

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
                double currentFloorX = weight * floorXWall + (1.0 - weight) * posX;
                double currentFloorY = weight * floorYWall + (1.0 - weight) * posY;

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
                int floorTexture = floorTiles[(int)currentFloorX][(int)currentFloorY];
                int ceilTexture = ceilTiles[(int)currentFloorX][(int)currentFloorY];

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
            spriteDistance[i] = ((posX - sprite[i].x) * (posX - sprite[i].x) + (posY - sprite[i].y) * (posY - sprite[i].y)); // sqrt not taken, unneeded
        }
        
        /* Then, sort the srites given the spriteOrder and spriteDistance (with numSprites)*/
        sortSprites(spriteOrder, spriteDistance, numSprites);

        /* take each sprite (based off the order), and then draw them */
        for (int i = 0; i < numSprites; i++)
        {
            /* define sprite location */
            double spriteX = sprite[spriteOrder[i]].x - posX;
            double spriteY = sprite[spriteOrder[i]].y - posY;


            
            double invDet = 1.0 / (planeX * dirY - dirX * planeY);

            /* factor to transform the sprite */
            double transformX = invDet * (dirY * spriteX - dirX * spriteY);
            double transformY = invDet * (-planeY * spriteX + planeX * spriteY);

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
        renderMap(map, 10, 32, 32 + mapPos, (int)posX, (int)posY, wallPosX, wallPosY);
        
        /* Debug Screen parameters */
        if (debugScreen == 1)
        {
            // every 8 pixels
            QuickCG::print("Debug Screen", 0, 0);
            QuickCG::print(1.0 / frameTime, 0, 8);
            QuickCG::print(dirX, 0, 16);
            QuickCG::print(dirY, 0, 24);
            QuickCG::print(posX, 0, 32);
            QuickCG::print(posY, 0, 40);
            QuickCG::print(posY, 0, 40);
            QuickCG::print(wallPosX, 0, 48);
            QuickCG::print(wallPosY, 0, 56);
            QuickCG::print(walls[(int)wallPosX][(int)wallPosY], 0, 64);
            QuickCG::print(wallDist, 0, 72);
        }

        /* Now, we re-draw the screen from scratch */
        QuickCG::redraw();

        double moveSpeed = frameTime * 3.0;
        double rotSpeed = frameTime * 2.0;

        /* GAME LOGIC GOES HERE */


        exploreMap(map, 3, (int)posX, (int)posY); //code for updating minimap 

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
                walls[(int)wallPosX][(int)wallPosY] = 0;
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
            if (walls[int(posX + dirX * moveSpeed)][int(posY)] == false)
                posX += dirX * moveSpeed;
            if (walls[int(posX)][int(posY + dirY * moveSpeed)] == false)
                posY += dirY * moveSpeed;
        }
        // move backwards if no wall behind you
        if (keyDown(SDLK_DOWN))
        {
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

/**
 * reveal tiles on a map. We take a radius, and then for that radius we mark tiles in the array based off of if they're a solid or not.
 *
 *  */
void exploreMap(int map[], int radius, int px, int py)
{
    

    /* Scans through full map */
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            //bounding parameters for the minimap explore radius 
            int x_bound_left = px - radius;
            int x_bound_right = px + radius;

            int y_bound_left = py - radius;
            int y_bound_right = py + radius;

            /* check if current pixel is within our map bounding box*/
            if (((x >= x_bound_left) && (x <= x_bound_right)) && ((y >= y_bound_left) && (y <= y_bound_right)))
            {
                /* then check if the current pixel is within bounds of the array, otherwise it will segfault */
                if (((x >= 0) && (x <= mapWidth)) && ((y >= 0) && (y <= mapWidth)))
                {

                    /* check for which tile is solid and which one is not. We update based off of which tile is which */
                    if (walls[x][y] == 0)
                    {
                        map[x * mapWidth + y] = 1;
                    }
                    else
                    {
                        map[x * mapWidth + y] = 0;
                    }
                }
            }
        }
    }
}

void playerInit()
{
    /* variations for sprites */
    int variation[] = {3, 4, 5, 6};
    
    /* Generates a new map with cellular automata */
    CellularAutomataGenerate(gen, buf, 0.5f, 1);
    /* Actually copies data over to the mapData arrays*/
    copyMap(gen, ceilTiles, walls, floorTiles);

    /* Generates locations of static props */
    generateStaticPositions(walls, sprite, variation);

    // rudimentary player positioning system which finds the first available position
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapWidth; y++) {
            if ((walls[x][y] == 0)) {
                posX = x + 0.5;
                posY = y + 0.5;
                
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