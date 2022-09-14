# Aeròlits
![CMake badge](https://github.com/lyquid/Aerolits/actions/workflows/build.yml/badge.svg)

Asteroids clone. 

Project mainly for learning and trying new stuff.

## Libraries used

 - [SDL2](https://www.libsdl.org/) for window creation and input handling. 
 - [SDL2_ttf](https://wiki.libsdl.org/SDL_ttf/FrontPage) for GUI's fonts rendering.    
 - [SDL2_mixer](https://wiki.libsdl.org/SDL_mixer/FrontPage) *not yet used :P* 
 - [Box2D](https://box2d.org/) for physics. 
 - [Dear ImGui](https://github.com/ocornut/imgui) for backend information.
 - [GLEW](http://glew.sourceforge.net/) for OpenGL context creation and functions. 
 - [glm](https://github.com/g-truc/glm) for algebra and camera.
 - [pugixml](https://pugixml.org/) to parse config files.
 - [stb_image](https://github.com/nothings/stb) for image loading.

## Features

 - Full physics simulation powered by Box2D!
 - Procedurally generated aerolites (asteroids)!
 - Custom particle system. You can mod it by simply editing the xml file! 
 - Entity Component System based on what I thought it should be an Entity Component System. Not very good at data locality, but does the job for this game.


#
> Written with [StackEdit](https://stackedit.io/).
