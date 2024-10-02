#include "minimap.h"

void exploreMap(int map[],int walls[], int radius, struct Player p)
{
    

    /* Scans through full map */
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            //bounding parameters for the minimap explore radius 
            int x_bound_left = ((int)p.posX) - radius;
            int x_bound_right = ((int)p.posX) + radius;

            int y_bound_left = ((int)p.posY) - radius;
            int y_bound_right = ((int)p.posY) + radius;

            /* check if current pixel is within our map bounding box*/
            if (((x >= x_bound_left) && (x <= x_bound_right)) && ((y >= y_bound_left) && (y <= y_bound_right)))
            {
                /* then check if the current pixel is within bounds of the array, otherwise it will segfault */
                if (((x >= 0) && (x <= mapWidth)) && ((y >= 0) && (y <= mapWidth)))
                {

                    /* check for which tile is solid and which one is not. We update based off of which tile is which */
                    if (walls[mapHeight * (x) + (y)] == 0)
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

void renderMap(int map[], int mapScale, int xPos, int yPos, double px, double py, double rx, double ry)
{

    int poffset = 16;
    int pageWidth = (mapScale * mapWidth)+poffset;
    int pageHeight = (mapScale * mapHeight)+poffset;
    QuickCG::ColorRGB beige = {250,233,181};
    QuickCG::drawRect(xPos-poffset,yPos-poffset-16,xPos+pageWidth,yPos+pageHeight, beige);
    QuickCG::print("Map-a-Tronic 2",xPos,yPos-8,RGB_Black);

    int grid = 2;
    for (int x = 0; x < mapHeight; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            
            if (map[x * mapHeight + y] == 1)
            {
                if ((x == (int)px) && (y == (int)py)) {
                    QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_LightRed);
                }else {
                    QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Green);
                }
            }else {
                QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Black);
            }
    
        }
    }
    QuickCG::drawLine(xPos + (px * mapScale), yPos + (py * mapScale), xPos + (px * mapScale) + (rx * 10), yPos + (py * mapScale) + (ry * 10), RGB_Red);
    //QuickCG::drawLine(xPos+px*mapScale,yPos+py*mapScale,xPos+rx*mapScale,yPos+ry*mapScale,RGB_Red);
}