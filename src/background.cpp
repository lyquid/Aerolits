#include "background.h"

ktp::Background::Background(const SDL_Point& screen_size): screen_size_(screen_size) {
  generateStars();
}

void ktp::Background::draw(SDL2_Renderer& renderer) const {
  renderer.setDrawColor(background_color_);
  renderer.drawRectFill(background_shape_);
  renderer.setDrawColor(255, 255, 255, 255);
  for (const auto& star: stars_) {
    renderer.setDrawColor(star.color_);
    renderer.drawPoint(star.position_);
  }
}

void ktp::Background::generateStars() {
  std::random_device seed;
  std::mt19937 generator(seed());
  std::uniform_int_distribution<unsigned int> distribution_stars(0u, 1000u);
  Star star;

  for (auto i = 0; i < screen_size_.x; ++i) {
    for (auto j = 0; j < screen_size_.y; ++j) {
      if (distribution_stars(generator) == 1) {
        star.position_.x = i;
        star.position_.y = j;
        star.color_ = {255, 255, 255, 255};
        stars_.push_back(star);
      }
    }
  }
}