#ifndef KTP_SDL2_WRAPPERS_SDL2_GLEW_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_GLEW_HPP_

#include <SDL.h>
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\GLU.h>

namespace ktp {

class SDL2_GLEW {

 public:

  bool init(SDL_Window* window);

 private:

  bool initGL();

  GLuint gProgramID = 0;
  GLint gVertexPos2DLocation = -1;
  GLuint gVBO = 0;
  GLuint gIBO = 0;

  SDL_GLContext context_ {NULL};
};

} // namespace ktp
#endif // KTP_SDL2_WRAPPERS_SDL2_GLEW_HPP_
