#ifndef _EFFECTS_
#define _EFFECTS_

#include "graphics.h"
#include <iostream>

using namespace QuickCG;
using namespace std;


/**
 * FOG ENGINE
 * The 'Fog Engine' is really just a pseudo lighting engine where a 'black fog' is drawn where the player looks
 * to simulate darkness. Lighting elements are slightly visible through this fog, which simulates a form of 'lights'.
 * This means maps can have some form of visibility. Not at all realistic, but it's doable...
 * Somewhay similar to sector lighting from DOOM.
 */

/* COLOR BANDING */
/**
 * broken fog system which uses bands of darkness to simulate a 'retro lighting' effect. Does not work currently,
 * but is faster than gradient-interpolation lighting.
 * around 4 values from lightness to darkness where a shading is applied
 * Utilize lookup table (really just a list of defines) with set shades of brightness
 */

#define FOG_BLACK 0x010101
#define FOG_DARK 0x4144959
#define FOG_NEUTRAL 0x8355711
#define FOG_LIGHT 0x12500670

inline Uint32 compute_fog_banded(double fogPercentage, Uint32 color);
inline Uint32 compute_fog(double fogPercentage, Uint32 color, Uint32 fog);
inline Uint32 compute_fog(double fogPercentage, Uint32 color, ColorRGB fog_rgb);
/**
 * Inline function compute_fog
 * inlined compute_fog function which calculates the correct fog value for each pixel based off of distance
 * Yes, it actually does save 5-10 fps on average while inlined
 */

/**
 * This does not fucking work... Or does it?  
 */
inline Uint32 compute_fog_banded(double fogPercentage, Uint32 color)
{

    int percent = fogPercentage * 100;
    int color_red, color_green, color_blue;
    if (percent >= 75)
    {
        color = compute_fog(fogPercentage, color, FOG_BLACK);
        // color = (color >> 1) & FOG_BLACK;
    }
    else if (percent >= 60)
    {
        color = compute_fog(fogPercentage, color, FOG_DARK);
        // color = (color >> 1) & FOG_DARK;
    }
    else if (percent >= 50)
    {
        color = compute_fog(fogPercentage, color, FOG_NEUTRAL);
        // color = (color >> 1) & FOG_NEUTRAL;
    }
    else if (percent >= 40)
    {
        color = compute_fog(fogPercentage, color, FOG_LIGHT);
        // color = (color >> 1) & FOG_LIGHT;
    }
    else
    {
        return color;
    }

    return color;
}

inline Uint32 compute_fog(double fogPercentage, Uint32 color, Uint32 fog)
{
    // break up texture into rgb values, then multiply fog
    // fog_rgb is derived from the very beginning of the program and is retrieved from there

    int color_red = (1.0f - fogPercentage) * Color_Red(color) + fogPercentage * Color_Red(fog);
    int color_green = (1.0f - fogPercentage) * Color_Green(color) + fogPercentage * Color_Green(fog);
    int color_blue = (1.0f - fogPercentage) * Color_Blue(color) + fogPercentage * Color_Blue(fog);

    // compose the colors again into one Uint32 color and then put that in the buffer
    color = Color_Composite(color_red, color_green, color_blue);

    return color;
}

inline Uint32 compute_fog(double fogPercentage, Uint32 color, ColorRGB fog_rgb)
{
    // break up texture into rgb values, then multiply fog
    // fog_rgb is derived from the very beginning of the program and is retrieved from there
    //we also make sure the color is not black, because otherwise the transparent parts turn black
    // (reminder that black is 0 0 0)
    if ((color & 0x00FFFFFF) != 0)
    {
        int color_red = (1.0f - fogPercentage) * Color_Red(color) + fogPercentage * fog_rgb.r;
        int color_green = (1.0f - fogPercentage) * Color_Green(color) + fogPercentage * fog_rgb.g;
        int color_blue = (1.0f - fogPercentage) * Color_Blue(color) + fogPercentage * fog_rgb.b;

        // compose the colors again into one Uint32 color and then put that in the buffer
        color = Color_Composite(color_red, color_blue, color_green);
    }
    return color;
}

//void renderMap(int map[], int mapScale, int xPos, int yPos, int px, int py, int rx, int ry);

#endif //_EFFECTS_