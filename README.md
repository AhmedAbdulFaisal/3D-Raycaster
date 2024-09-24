Here is a full Raycaster made with SDL C++. This isn't necessarily actual 3D, but is pseudo-2D rendering

The rendering code was mostly generated from https://lodev.org/cgtutor/raycasting.html, which I would recommend EVERYONE to see.

Additions:
* Make the game less dependent on Graphics.h and Graphics.cpp (which was copied over from the lodev tutorial), and make a GPU-based rendering system while keeping the pixel based buffering intact
* Possibly move over to SDL 2, this uses SDL 1.2
* Streamlining of rendering code
* More level features such as variable heights (for water), sloped walls
* Background rendering


What the game looks like:


Changelog: v 0.2:

*Added random generation
*Added textures, sprites, and raycster engine
*Fog


![image](https://github.com/user-attachments/assets/bff9d309-eeb8-4c8d-81ad-9c78948cb38e)


Changelog v 0.3:

* Added a minimap which can be seen by holding down [TAB]. Holding [F3] 
* The levels can now be randomly generated at the press of a button - just press [E] and the level will re-generate, with a new map and all
* Variation in wall-tiles and sprites, now you have randomly generated cave decor and 'weak' walls are indicated by walls with cracks. 
* You can break blocks near you! When you're in front of a wall (within approx 1 block), press [SPACE] to remove it. Borders are placed at the end of the wall to prevent you from breaking beyond the barrier (where it segfaults).

![image](https://github.com/user-attachments/assets/4d1403cf-768d-4740-89b7-5f0ab9681084)


What to fix:

* Minimap still shows previous 'data' which is replaced when the player moves around upon new map loading - make it so that the map clears when the player enters a new level
* Random placement system is pretty bad, make it so that it's actually random instead of just finding the first empty space
* Refactor code
* The beige background which is slid out when the map is displayed doesn't really slide out, it only draws when the animation is finished. Either attempt to fix it or keep it in. 



