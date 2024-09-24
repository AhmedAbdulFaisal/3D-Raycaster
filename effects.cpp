#include "effects.h"

void renderMap(int map[], int mapScale, int xPos, int yPos, int px, int py, int rx, int ry)
{

    int poffset = 16;
    int pageWidth = (mapScale * length)+poffset;
    int pageHeight = (mapScale * length)+poffset;
    QuickCG::ColorRGB beige = {250,233,181};
    QuickCG::drawRect(xPos-poffset,yPos-poffset-16,xPos+pageWidth,yPos+pageHeight, beige);
    QuickCG::print("Map-a-Tronic 2",xPos,yPos-8,RGB_Black);

    int grid = 2;
    for (int x = 0; x < length; x++)
    {
        for (int y = 0; y < length; y++)
        {
            
            if (map[x * length + y] == 1)
            {
                if ((x == px) && (y == py)) {
                    QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Red);
                }else {
                    QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Green);
                }
            }else {
                QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Black);
            }
    
        }
    }

    QuickCG::drawLine(xPos+px*mapScale,yPos+py*mapScale,xPos+rx*mapScale,yPos+ry*mapScale,RGB_Red);
}

void renderMap(int map[24][24], int mapScale, int xPos, int yPos, int px, int py, int rx, int ry)
{

    int poffset = 16;
    int pageWidth = (mapScale * length)+poffset;
    int pageHeight = (mapScale * length)+poffset;
    QuickCG::ColorRGB beige = {250,233,181};
    QuickCG::drawRect(xPos-poffset,yPos-poffset-16,xPos+pageWidth,yPos+pageHeight, beige);
    QuickCG::print("Map-a-Tronic 2",xPos,yPos-8,RGB_Black);

    int grid = 2;
    for (int x = 0; x < length; x++)
    {
        for (int y = 0; y < length; y++)
        {
            
            if (map[x][y] == 1)
            {
                if ((x == px) && (y == py)) {
                    QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Red);
                }else {
                    QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Green);
                }
            }else {
                QuickCG::drawRect(xPos + (x * mapScale),yPos + (y * mapScale),xPos + ((x * mapScale) + mapScale-grid),yPos +  ((y * mapScale) + mapScale-grid), RGB_Black);
            }
    
        }
    }

    QuickCG::drawLine(xPos+px*mapScale,yPos+py*mapScale,xPos+rx*mapScale,yPos+ry*mapScale,RGB_Red);
}