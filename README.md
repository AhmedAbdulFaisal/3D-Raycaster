Here is a full Raycaster made with SDL C++. This isn't necessarily actual 3D, but is pseudo-2D rendering

The rendering code was mostly generated from https://lodev.org/cgtutor/raycasting.html, which I would recommend EVERYONE to see.

Currently the engine can do the following
* Randomly generated levels with cellular automata (generator.cpp / generator.h)
* Relatively fast fog rendering which acts as lighting
* Everything from the raycasting tutorial, such as textured ceilings, floors, and walls.

This will be turned into a roguelike video game in the future

Additions:
* Make the game less dependent on Graphics.h and Graphics.cpp (which was copied over from the lodev tutorial), and make a GPU-based rendering system while keeping the pixel based buffering intact
* Possibly move over to SDL 2, this uses SDL 1.2
* Streamlining of rendering code
* More level features such as variable heights (for water), sloped walls
* Background rendering


What the game looks like:


![image](https://github.com/user-attachments/assets/bff9d309-eeb8-4c8d-81ad-9c78948cb38e)


