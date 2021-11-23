#pragma once

#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"

namespace ktp {

class Testing {
 public:
  Testing();
  ~Testing() { glDeleteProgram(gProgramID_); }
  void draw() const;
  bool initGL();

 private:

  GLuint gProgramID_ = 0;
  GLint gVertexPos2DLocation_ = -1;
  GLuint gVBO_ = 0;
  GLuint gIBO_ = 0;

  bool gRenderQuad_ = true;
};

} // namespace ktp
