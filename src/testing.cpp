#include "include/palette.hpp"
#include "include/testing.hpp"

void ktp::Testing::draw(const SDL2_Renderer& ren) const {
  ren.setDrawColor(Colors::copper_green);
  for (const auto& point: polygon_) ren.drawLine(center_, point);
  ren.setDrawColor(Colors::red);
  ren.drawPoint(center_);
  ren.drawLines(polygon_);
}

void ktp::Testing::generateShape(float max_size, int sides) {
  if (sides < 3) return;
  if (!polygon_.empty()) polygon_.clear();
  SDL_Point point {};
  for (auto i = 0; i < sides; ++i) {
    const auto f_size {max_size * generateRand(0.85f, 1.f)};
    point.x = f_size * SDL_cosf(2 * kPI * i / sides) + center_.x;
    point.y = f_size * SDL_sinf(2 * kPI * i / sides) + center_.y;
    polygon_.push_back(point);
  }
  // this is the closing point == first point
  polygon_.push_back(polygon_.front());
}