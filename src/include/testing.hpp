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

  // Testing();
  // ~Testing() {}
  void draw() const;
  void init();
  void update(float delta_time);

 private:

  VAO vao_ {};
  VBO vbo_ {};
  VBO uv_ {};
  GLfloatVector vertices_ {};
  GLfloatVector vertices_color_ {};
  std::vector<bool> vertices_color_direction_ {};
  Texture2D texture_ {};

  ShaderProgram shader_program_ {};

  const glm::mat4 projection_ {glm::perspective(glm::radians(45.f), 1366.f / 768.f, 0.1f, 100.f)};
  GLfloat angle_ {10.f};

  glm::vec3 camera_pos_ {0.f, 0.f, 3.f};
  glm::vec3 camera_front_ {0.f, 0.f, -1.f};
  glm::vec3 camera_up_ {0.f, 1.f, 3.f};
  glm::mat4 view_ {glm::lookAt(camera_pos_,
                               camera_pos_ + camera_front_,
                               camera_up_)};
  glm::mat4 mvp_ {};
};

} // namespace ktp
