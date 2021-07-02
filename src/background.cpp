#include "include/background.hpp"
#include "include/box2d_scale.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include <ctime>
#include <random>

/* GRAPHICS */

SDL_Rect ktp::BackgroundGraphicsComponent::shapeToRect(const FPointsVector& render_shape) {
  return SDL_Rect {
    (int)render_shape[0].x,
    (int)render_shape[0].y,
    (int)render_shape[1].x - (int)render_shape[0].x,
    (int)render_shape[3].y - (int)render_shape[0].y
  };
}

void ktp::BackgroundGraphicsComponent::update(const GameEntity& background, const SDL2_Renderer& renderer) {
  renderer.setDrawColor(kBackgroundColor_);
  renderer.drawRectFill(shapeToRect(render_shape_));
  for (const auto& star: stars_) {
    renderer.setDrawColor(star.color_);
    renderer.drawPoint(star.position_);
  }
}

/* PHYSICS */

ktp::BackgroundPhysicsComponent::BackgroundPhysicsComponent(BackgroundGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  graphics_->render_shape_.push_back({-kXtraSpace_, -kXtraSpace_}); // top left
  graphics_->render_shape_.push_back({b2_screen_size_.x * kMetersToPixels + kXtraSpace_, -kXtraSpace_}); // top right
  graphics_->render_shape_.push_back({b2_screen_size_.x * kMetersToPixels + kXtraSpace_, b2_screen_size_.y * kMetersToPixels + kXtraSpace_}); // bottom right
  graphics_->render_shape_.push_back({-kXtraSpace_, b2_screen_size_.y * kMetersToPixels + kXtraSpace_}); // bottom left
  graphics_->render_shape_.push_back({-kXtraSpace_, -kXtraSpace_}); // top left again
  generateStars();
}

ktp::BackgroundPhysicsComponent& ktp::BackgroundPhysicsComponent::operator=(BackgroundPhysicsComponent&& other) noexcept {
  if (this != &other) {
    graphics_ = other.graphics_;
    other.graphics_ = nullptr; // just in case
  }
  return *this;
}

void ktp::BackgroundPhysicsComponent::generateStars() {
  #if __GNUC__ || __MINGW32__
    std::mt19937 generator(time(nullptr));
  #else
    std::random_device seed; // this gives the same output in GCC 8.1.0
    std::mt19937 generator(seed());
  #endif
  std::uniform_int_distribution<unsigned int> distribution_stars(0u, 6000u);
  std::uniform_int_distribution<unsigned int> distribution_colors(0u, graphics_->star_colors_.size() - 1);
  std::uniform_real_distribution<float> distribution_delta(0.001f, 0.1f);
  Star star;

  for (auto i = 0; i < (int)(b2_screen_size_.x * kMetersToPixels); ++i) {
    for (auto j = 0; j < (int)(b2_screen_size_.y * kMetersToPixels); ++j) {
      if (distribution_stars(generator) == 1) {
        star.position_.x = (float)i;
        star.position_.y = (float)j;
        star.delta_ = {0.f, distribution_delta(generator)};
        if (graphics_->stars_.size() % 2 == 0 && graphics_->stars_.size() % 3 == 0) {
          star.color_ = graphics_->star_colors_[distribution_colors(generator)];
        } else {
          star.color_ = Colors::white;
        }
        graphics_->stars_.push_back(std::move(star));
      }
    }
  }
}

void ktp::BackgroundPhysicsComponent::update(const GameEntity& background, float delta_time) {
  for (auto& star: graphics_->stars_) {
    if (star.position_.y >= b2_screen_size_.y * kMetersToPixels) {
      star.position_.y = -kXtraSpace_;
    } else {
      star.position_.y += star.delta_.y * delta_time;
    }
  }
}