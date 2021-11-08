#pragma once

#include "random.hpp"
#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include <SDL.h>
#include <vector>

namespace ktp {

class Testing {
 public:
  Testing();
  void draw(const SDL2_Renderer& ren) const;
  inline void generateShape(float max_size) { generateShape(max_size, generateRand(15, 20)); }
  void generateShape(float max_size, int sides);
  inline void drawTriangles() { draw_triangles_ = !draw_triangles_; }

 private:

  Geometry::Polygon polygon_ {};
  Geometry::Polygon test_polygon_ {};
  std::vector<Geometry::Polygon> triangles_ {};
  std::vector<Geometry::Polygon> test_triangles_ {};

  static constexpr auto kPI {3.14159265359};
  const SDL_FPoint center_ {683, 384};
  const SDL_FPoint test_center_ {50, 50};
  bool draw_triangles_ {};
};

} // namespace ktp
