#pragma once

#include "camera.hpp"
#include "opengl.hpp"
#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace ktp {

class Testing {
 public:

  void draw();
  void init();
  void update(float delta_time);
  void updateMouse(float x_pos, float y_pos) { camera_.look(x_pos, -y_pos); }
  void updateZoom(float y_offset) { camera_.zoom(y_offset); }

 private:

  void updateCamera(float delta_time);
  void updateMVP();

  static constexpr int kNumCubes_ {1000};

  VAO vao_ {};
  VBO vertices_ {};
  VBO colors_ {};
  VBO translations_ {};
  GLfloatVector vertices_data_ {};
  ShaderProgram shader_program_ {};

  Camera camera_ {glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f};
};

} // namespace ktp
