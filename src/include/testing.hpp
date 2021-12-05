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

  VAO vao1_ {};
  VAO_Config vao1_config_{};
};

} // namespace ktp
