#include "include/background.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include <random>

ktp::Background::Background(const SDL_Point& screen_size): screen_size_(screen_size) {
  generateStars();
}

void ktp::Background::draw(const SDL2_Renderer& renderer) const {
  renderer.setDrawColor(background_color_);
  renderer.drawRectFill(background_shape_);
  for (const auto& star: stars_) {
    renderer.setDrawColor(star.color_);
    renderer.drawPoint(star.position_);
  }
}

void ktp::Background::generateStars() {
  std::random_device seed;
  std::mt19937 generator(seed());
  std::uniform_int_distribution<unsigned int> distribution_stars(0u, 6000u);
  std::uniform_int_distribution<unsigned int> distribution_colors(0u, star_colors_.size() - 1);
  std::uniform_real_distribution<float> distribution_delta(0.001f, 0.1f);
  Star star;

  for (auto i = 0; i < screen_size_.x; ++i) {
    for (auto j = 0; j < screen_size_.y; ++j) {
      if (distribution_stars(generator) == 1) {
        star.position_.x = i;
        star.position_.y = j;
        star.delta_ = {0.f, distribution_delta(generator)};
        if (stars_.size() % 2 == 0 && stars_.size() % 3 == 0) {
          star.color_ = star_colors_[distribution_colors(generator)];
        } else {
          star.color_ = Colors::white;
        }
        stars_.push_back(star);
      }
    }
  }
}

void ktp::Background::update(float delta_time) {
  for (auto& star: stars_) {
    if (star.position_.y >= screen_size_.y) {
      star.position_.y = 0.f;
    } else {
      star.position_.y += star.delta_.y * delta_time;
    }
  }
}