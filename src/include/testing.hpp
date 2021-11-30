#pragma once

#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace ktp {

struct VAO {
  GLuint id;
  // GLfloat* vertex_buffer_data; // the coords array
  std::vector<GLfloat> vertex_buffer_data; // idem with a vector
  GLuint vertex_buffer;
};

class Testing {
 public:
  Testing();
  ~Testing() {}
  void draw() const;
  void tutorial();
  void update(float delta_time);

 private:
  // cube
  GLuint vertex_array_id_ {};
  GLuint vertex_buffer_ {};
  GLuint color_buffer_ {};
  const std::vector<GLfloat> vertex_buffer_data_ {SDL2_GLEW::cube()};
  std::vector<GLfloat> color_buffer_data_ {};
  std::vector<bool> color_buffer_data_pattern_ {};
  GLProgram program_ {};

  glm::mat4 projection_ {};
  glm::mat4 view_ {};
  // Model matrix: an identity matrix (model will be at the origin)
  glm::mat4 model_ {glm::mat4{1.0f}};
  glm::mat4 mvp_ {};
  GLuint matrix_id_ {};



  // triangle
  // GLuint tri_vertex_array_id_ {};
  // GLuint tri_vertex_buffer_ {};
  // const std::vector<GLfloat> tri_vertex_buffer_data_ {
  //  -1.0f, -1.0f, 0.0f,
  //   1.0f, -1.0f, 0.0f,
  //   0.0f,  1.0f, 0.0f,
  // };
  // glm::mat4 tri_mvp_ {};
};

} // namespace ktp
