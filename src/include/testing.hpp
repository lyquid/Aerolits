#pragma once

#include "camera.hpp"
#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include "../../lib/imgui/imgui.h"
#include "../../lib/imgui/imgui_impl_sdl.h"
#include "../../lib/imgui/imgui_impl_opengl3.h"
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
  void updateMouse(float x_pos, float y_pos) { camera_.look(x_pos, -y_pos); }
  void updateZoom(float y_offset) { camera_.zoom(y_offset); }

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

  Camera camera_ {glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 1.f, 0.f), -90.f, 0.f};
};

} // namespace ktp
