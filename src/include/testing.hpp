#pragma once

#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ktp {

class Testing {
 public:
  Testing();
  ~Testing() { glDeleteProgram(gProgramID_); }
  void draw() const;
  void draw_OLD() const;
  bool initGL();
  void tutorial();

 private:

  GLuint gProgramID_ = 0;
  GLint gVertexPos2DLocation_ = -1;
  GLuint gVBO_ = 0;
  GLuint gIBO_ = 0;
  bool gRenderQuad_ = true;

  // tutorial
  GLuint vertex_array_id_ {};
  GLuint vertex_buffer_ {};
  GLuint program_id_ {};

  glm::mat4 projection_ {};
  glm::mat4 view_ {};
  // Model matrix : an identity matrix (model will be at the origin)
  glm::mat4 model_ {glm::mat4{1.0f}};
  glm::mat4 mvp_ {};
  GLuint matrix_id_ {};
};

} // namespace ktp
