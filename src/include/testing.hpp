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

  //VAO cube_ {};
  /// VAO_Config cube_config_{};
  std::vector<GLfloat> color_vertices_ {};
  std::vector<bool> color_vertices_direction_ {};

  ShaderProgram shader_program_ {};

  glm::mat4 model_ {glm::mat4(1.f)};
  const glm::mat4 projection_ {glm::perspective(glm::radians(45.f), 1366.f / 768.f, 0.1f, 100.f)};
  GLfloat angle_ {10.f};

  glm::vec3 camera_pos_ {0.f, 0.f, 3.f};
  glm::vec3 camera_front_ {0.f, 0.f, -1.f};
  glm::vec3 camera_up_ {0.f, 1.f, 3.f};
  glm::mat4 view_ {glm::lookAt(camera_pos_,
                               camera_pos_ + camera_front_,
                               camera_up_)};
};

} // namespace ktp
