![CMake badge](https://github.com/lyquid/Aerolits/actions/workflows/build.yml/badge.svg)
# Aeròlits
Asteroids clone. 
Project mainly for learning and trying new stuff.

It started as a simple project to try out SDL2.

The ship was just a bunch of points united by lines without even filling up the shape, and the aerolites were just circles made with the midpoint circle algorithm.

Elastic collision algorithm was used for aerolites bouncing into another. Chaos was ensured when they wrapped into another one.

Eventually the aerolites become more than a circle, so a more advanced physics system was needed. Box2D was introduced.
Then the particles. How fun was to code and mess with the config to get ridiculous effects.

The last improvement was porting from the SDL renderer to OpenGL, which gave the ability to texture map the aerolites and fill the polygonal player's ship. Also improved the particles' performance with instanced rendering.

## Libraries used
 - [Box2D](https://box2d.org/) for the physics. 
 - [Dear ImGui](https://github.com/ocornut/imgui) for backend information and debugging.
 - [GLEW](http://glew.sourceforge.net/) for OpenGL context creation and functions. 
 - [glm](https://github.com/g-truc/glm) for algebra and camera utilities.
 - [pugixml](https://pugixml.org/) to parse the config files.
 - [SDL2](https://www.libsdl.org/) for window creation and input handling. 
 - [SDL2_ttf](https://wiki.libsdl.org/SDL_ttf/FrontPage) for fonts rendering.    
 - [SDL2_mixer](https://wiki.libsdl.org/SDL_mixer/FrontPage) *not yet used :P* 
 - [stb_image](https://github.com/nothings/stb) for image loading.

## Features

 - Full physics simulation powered by Box2D!
 - Procedurally generated aerolites (asteroids)!
 - Custom particle system. You can mod it by simply editing the xml file! 
 - Entity Component System based on what I thought it should be an Entity Component System. Not very good at data locality, but does the job for this game.


#
> Written with [StackEdit](https://stackedit.io/).
