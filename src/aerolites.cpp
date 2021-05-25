#include "aerolites.hpp"

ktp::Aerolite ktp::Aerolite::spawnAerolite(const SDL_Point& screen_size) {
  setScreenSize(screen_size);
  Aerolite aerolite {};
  const auto final_size {kMaxSize_ * generateRand(0.3f, 1.f)};
  SpawnSides spawn_side {generateRand(0, 3)};
  //logMessage(std::to_string(spawn_side));
  switch (spawn_side) {
    case SpawnSides::Top:
      aerolite.position_.x = static_cast<float>(screen_size.x) * generateRand(0.f, 0.8f);
      aerolite.position_.y = 0 - final_size;
      aerolite.delta_ = {0, 1};
      break;
    case SpawnSides::Right:
      aerolite.position_.x = static_cast<float>(screen_size.x) + 1;
      aerolite.position_.y = static_cast<float>(screen_size.y) * generateRand(0.f, 0.8f);
      aerolite.delta_ = {-1, 0};
      break;
    case SpawnSides::Down:
      aerolite.position_.x = static_cast<float>(screen_size.x) * generateRand(0.f, 0.8f);
      aerolite.position_.y = static_cast<float>(screen_size.y) + 1;
      aerolite.delta_ = {0, -1};
      break;
    case SpawnSides::Left:
      aerolite.position_.x = 0 - final_size;
      aerolite.position_.y = static_cast<float>(screen_size.y) * generateRand(0.f, 0.8f);
      aerolite.delta_ = {1, 0};
      break;
    default:
      break;
  }
  aerolite.shape_.x = aerolite.position_.x;
  aerolite.shape_.y = aerolite.position_.y;
  aerolite.shape_.w = final_size;
  aerolite.shape_.h = final_size;
  
  return aerolite;
}

void ktp::Aerolite::draw(const SDL2_Renderer& renderer) const {
  renderer.drawRect(shape_);
}

void ktp::Aerolite::update(float delta_time) {
  constexpr auto threshold {20};
  position_.x += delta_.x * delta_time * speed_;
  position_.y += delta_.y * delta_time * speed_;
  if (position_.x + shape_.w < -threshold || position_.x > kScreenSize.x + threshold 
   || position_.y + shape_.h < -threshold || position_.y > kScreenSize.y + threshold) {
    to_be_deleted_ = true;
  } else {
    shape_.x = position_.x;
    shape_.y = position_.y;
  }
}