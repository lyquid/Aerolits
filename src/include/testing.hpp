#pragma once

#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace ktp {

class Testing {
 public:

  Testing();
  ~Testing() {}
  void draw() const;
  void tutorial();
  void update(float delta_time);

 private:

  VAO cube_ {};
  VAO_Config cube_config_{};

  ShaderProgram shader_program_ {};

  glm::mat4 model_ {glm::mat4(1.f)};
  glm::mat4 view_ {glm::mat4(1.f)};
  glm::mat4 projection_ {glm::mat4(1.f)};
  GLfloat angle_ {10.f};
};

} // namespace ktp
