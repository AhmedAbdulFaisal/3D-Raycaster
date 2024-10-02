
#include "renderer.h"
#include "graphics.h"
#include <math.h>

using namespace std;
using namespace QuickCG;

void Renderer::clScreen()
{
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            buffer[y][x] = 0; // clear the buffer instead of cls()
}

void Renderer::drawScene(std::vector<Uint32> texture[256])
{
    /* uses re-written form of lodev's algorithim */
    /* RENDERING CODE */
    for (int x = 0; x < w; x++)
    {
        /* WALL CASTING CODE */
        double cam_x = 2 * x / double(w) - 1;
        double rdirx = p.dirX + p.planeX * cam_x;
        double rdiry = p.dirY + p.planeY * cam_x;

        /**
         * sideDistX/Y are the distance the ray has to travel for the
         * first x/y side encountered. deltaDistX/Y is the distance needed
         * for the next x/y side encountered. This repeats within a while
         * loop until a hit is found, where then these two lenghts are
         * calculated into a single line.
         **/

        int mapX = int(p.xPos);
        int mapY = int(p.yPos);

        double sidex, sidey;

        double deltadx = (rdirx == 0) ? 1e30 : std::abs(1 / rdirx);
        double deltady = (rdiry == 0) ? 1e30 : std::abs(1 / rdiry);
        double perpdist;

        int stepX, stepY;
        int hit = 0, side;

        /* get initial sideDist and step*/

        if (rdirx < 0)
        {
            stepX = -1;
            sidex = (p.xPos - mapX) * deltadx;
        }
        else
        {
            stepX = 1;
            sidex = (mapX + 1.0 - p.xPos) * deltadx;
        }
        if (rdiry < 0)
        {
            stepY = -1;
            sidey = (p.yPos - mapY) * deltady;
        }
        else
        {
            stepY = 1;
            sidey = (mapY + 1.0 - p.yPos) * deltady;
        }

        /* Actual DDA algorithim */

        while (hit == 0)
        {
            if (sidex < sidey)
            {
                sidex += deltadx;
                mapX += stepX;
                side = 0; // we hit the horizontal side
            }
            else
            {
                sidey += deltady;
                mapY += stepY;
                side = 1; // we hit the vertical side
            }
            if (wld.walls[mapX * MAPSIZE + mapY] > 0)
            {
                hit = 1;
            }
        }

        /**
         * So here we don't use the euclidean distance to calculate the
         * distance from the wall, we actually use the camera plane. This
         * prevents the fisheye effect which is caused by using the distance
         * from the player, instead of the perpendicular distance. Hence the
         * term, 'Perpendicular Player Distance', or perpdist.
         */

        if (side == 0)
        {
            perpdist = (sidex - deltadx);
        }
        else
        {
            perpdist = (sidey - deltady);
        }

        int lheight = (int)(h / perpdist);

        int dstart = -lheight / 2 + h / 2;
        if (dstart < 0)
        {
            dstart = 0;
        }
        int dend = lheight / 2 + h / 2;

        if (dend >= h)
        {
            dend = h - 1;
        }
        /* debug functions */
        // printf("PERPENDICULAR DIST: %f\n",perpdist);
        // printf("START/END: [%d,%d]\n",dstart,dend);

        /* TEXTURE RENDERING CODE (FOR WALLS) */

        int texNum = wld.walls[mapX * MAPSIZE + mapY] - 1;

        double wallX;

        if (side == 0)
        {
            wallX = p.yPos + perpdist * rdiry;
        }
        else
        {
            wallX = p.xPos + perpdist * rdirx;
        }
        wallX -= floor((wallX));

        int texX = int(wallX * double(TEXWIDTH));
        if (side == 0 && rdirx > 0)
        {
            texX = TEXWIDTH - texX - 1;
        }
        if (side == 1 && rdirx < 0)
        {
            texX = TEXWIDTH - texX - 1;
        }

        /* send data to main here */

        double step = 1.0 * TEXHEIGHT / lheight;
        double texPos = (dstart - h / 2 + lheight / 2) * step;
       
        for (int y = dstart; y < dend; y++)
        {
            
            int texY = (int)texPos & (TEXHEIGHT - 1);
            texPos += step;

            Uint32 color = texture[texNum][TEXHEIGHT * texY + texX];

            if (side == 1)
                color = (color >> 1) & 8355711;

            buffer[x][y] = color;
        }

        
    }
    QuickCG::drawBuffer(buffer[0]);
}

void Renderer::drawHud()
{
    drawMinimap(wld.walls, 20, p, 64, 64);
}

void Renderer::drawMinimap(int map[], int mapScale, struct Player pstruct, int xPos, int yPos)
{
    int grid = 2;
    for (int x = 0; x < 6; x++)
    {
        for (int y = 0; y < 6; y++)
        {

            if (map[x * 6 + y] == 1)
            {
                QuickCG::drawRect(xPos + (x * mapScale), yPos + (y * mapScale), xPos + ((x * mapScale) + mapScale - grid), yPos + ((y * mapScale) + mapScale - grid), RGB_Green);
            }
            else
            {
                QuickCG::drawRect(xPos + (x * mapScale), yPos + (y * mapScale), xPos + ((x * mapScale) + mapScale - grid), yPos + ((y * mapScale) + mapScale - grid), RGB_White);
            }
        }
    }
    QuickCG::drawLine(xPos + (pstruct.xPos * mapScale), yPos + (pstruct.yPos * mapScale), xPos + (pstruct.xPos * mapScale) + (pstruct.dirX * 10), yPos + (pstruct.yPos * mapScale) + (pstruct.dirY * 10), RGB_Red);
}
