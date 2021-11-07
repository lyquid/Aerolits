#include "include/palette.hpp"
#include "include/testing.hpp"
#include "sdl2_wrappers/sdl2_geometry.hpp"
#include <iostream>

void ktp::Testing::draw(const SDL2_Renderer& ren) const {
  ren.setDrawColor(Colors::red);
  ren.drawLines(polygon_);
  if (draw_triangles_) {
    ren.setDrawColor(Colors::copper_green);
    for (const auto& triangle: triangles_) {
      ren.drawLines(triangle);
    }
  }
}

void ktp::Testing::generateShape(float max_size, int sides) {
  if (sides < 3) return;
  polygon_.clear();
  result_.clear();
  triangles_.clear();

  SDL_FPoint point {};
  for (auto i = 0; i < sides; ++i) {
    const auto f_size {max_size * generateRand(0.8f, 1.f)};
    point.x = f_size * SDL_cosf(2 * kPI * i / sides) + center_.x;
    point.y = f_size * SDL_sinf(2 * kPI * i / sides) + center_.y;
    polygon_.push_back({point.x, point.y});
  }
  Geometry::triangulate(polygon_, triangles_, true);
  // this is the closing point == first point
  polygon_.push_back(polygon_.front());

  std::cout << "Polygon 1 area: " << std::to_string(Geometry::area(polygon_)) << '\n';

  // polygon_.push_back( {0 * center_.x,6 * center_.y});
  // polygon_.push_back( {0 * center_.x,0 * center_.y});
  // polygon_.push_back( {3 * center_.x,0 * center_.y});
  // polygon_.push_back( {4 * center_.x,1 * center_.y});
  // polygon_.push_back( {6 * center_.x,1 * center_.y});
  // polygon_.push_back( {8 * center_.x,0 * center_.y});
  // polygon_.push_back( {12 * center_.x,0 * center_.y});
  // polygon_.push_back( {13 * center_.x,2 * center_.y});
  // polygon_.push_back( {8 * center_.x,2 * center_.y});
  // polygon_.push_back( {8 * center_.x,4 * center_.y});
  // polygon_.push_back( {11 * center_.x,4 * center_.y});
  // polygon_.push_back( {11 * center_.x,6 * center_.y});
  // polygon_.push_back( {6 * center_.x,6 * center_.y});
  // polygon_.push_back( {4 * center_.x,3 * center_.y});
  // polygon_.push_back( {2 * center_.x,6 * center_.y});
  // Geometry::triangulate(polygon_, triangles_);
  // polygon_.push_back(polygon_.front());
}