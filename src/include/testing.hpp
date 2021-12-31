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

  void draw() const;
  void init();
  void update(float delta_time);
  void updateMouse(float x_pos, float y_pos);

 private:

  void updateCamera(float delta_time);
  void updateMVP(float delta_time);

  VAO vao_ {};
  VBO vertices_ {};
  VBO colors_ {};
  EBO indices_ {};
  GLfloatVector vertices_data_ {};
  GLuintVector indices_data_ {};
  GLfloatVector vertices_color_data_ {};
  Texture2D texture_ {};

  ShaderProgram shader_program_ {};

  const glm::mat4 projection_ {glm::perspective(glm::radians(45.f), 16.f / 9.f, 0.1f, 100.f)};

  glm::vec3 camera_pos_ {0.f, 0.f, 3.f};
  glm::vec3 camera_front_ {0.f, 0.f, -1.f};
  glm::vec3 camera_up_ {0.f, 1.f, 0.f};
  glm::mat4 view_ {glm::lookAt(camera_pos_,
                               camera_pos_ + camera_front_,
                               camera_up_)};
  glm::mat4 mvp_ {};

  float yaw_ {-90.f};
  float pitch_ {0.f};
};

} // namespace ktp
