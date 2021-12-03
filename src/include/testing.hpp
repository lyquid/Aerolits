#pragma once

#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

  SDL2_VAO vao1_ {};
  SDL2_GL::VAO_Config vao1_config_{};
  SDL2_VAO vao2_ {};
  SDL2_GL::VAO_Config vao2_config_{};
};

} // namespace ktp
