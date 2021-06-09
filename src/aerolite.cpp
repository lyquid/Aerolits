#include "include/aerolite.hpp"
#include "include/box2d_scale.hpp"
#include "include/palette.hpp"
#include "include/random.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include <utility> // std::exchange, std::swap

ktp::Aerolite::Aerolite(const SDL_FPoint& where) noexcept {
  const auto size {kMaxSize_ * generateRand(0.3f, 1.f)};
  generateAeroliteShape(size);

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.position.Set(where.x, where.y);
  
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

  body_ = b2_world_->CreateBody(&body_def);

  b2PolygonShape dynamic_box {};
  dynamic_box.SetAsBox(size / 2.f, size / 2.f);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &dynamic_box;
  fixture_def.density = kKgPerMeter2_ / (size * size);
  fixture_def.friction = 1.f;

  body_->CreateFixture(&fixture_def);
}

ktp::Aerolite::Aerolite(Aerolite&& other) noexcept {
  body_ = std::exchange(other.body_, nullptr);
  shape_ = other.shape_;
  render_shape_ = other.render_shape_;
  to_be_deleted_ = other.to_be_deleted_;
}

ktp::Aerolite& ktp::Aerolite::operator=(Aerolite&& other) noexcept {
  if (this != &other) {
    if (body_ != nullptr) {
      b2_world_->DestroyBody(body_);
      body_ = nullptr;
    }
    std::swap(body_, other.body_);
    shape_ = other.shape_;
    render_shape_ = other.render_shape_;
    to_be_deleted_ = other.to_be_deleted_;
  }
  return *this;
}

ktp::Aerolite ktp::Aerolite::spawnAerolite() {
  static int side {};
  const float delta {kMaxSpeed_ * generateRand(0.1f, 1.f)};
  Aerolite aerolite({static_cast<float>(screen_size_b2_.x) / 2, static_cast<float>(screen_size_b2_.y) / 2});
  aerolite.body_->SetAngularVelocity(kMaxRotationSpeed_ * generateRand(-1.f, 1.f));
  switch (side) {
    case 0: // up
      aerolite.body_->SetTransform({static_cast<float>(screen_size_b2_.x) / 2, 0.f}, aerolite.body_->GetAngle());      
      aerolite.body_->SetLinearVelocity({0, delta});
      ++side;
      break;
    case 1: // right
      aerolite.body_->SetTransform({static_cast<float>(screen_size_b2_.x), static_cast<float>(screen_size_b2_.y) / 2}, aerolite.body_->GetAngle());
      aerolite.body_->SetLinearVelocity({-delta, 0});
      ++side;
      break;
    case 2: // down
      aerolite.body_->SetTransform({static_cast<float>(screen_size_b2_.x) / 2, static_cast<float>(screen_size_b2_.y)}, aerolite.body_->GetAngle());
      aerolite.body_->SetLinearVelocity({0, -delta});
      ++side;
      break;
    case 3: // left
      aerolite.body_->SetTransform({0.f, static_cast<float>(screen_size_b2_.y) / 2}, aerolite.body_->GetAngle());
      aerolite.body_->SetLinearVelocity({delta, 0});
      side = 0;
      break;
  }
  return aerolite;
}

void ktp::Aerolite::draw(const SDL2_Renderer& renderer) const {
  renderer.setDrawColor(Colors::orange);
  renderer.drawLines(render_shape_);
  //renderer.setDrawColor(Colors::yellow);
  //renderer.drawPoint(body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels);
}

void ktp::Aerolite::generateAeroliteShape(float size) {
  shape_.push_back({-size / 2, -size / 2});
  shape_.push_back({ size / 2, -size / 2});
  shape_.push_back({ size / 2,  size / 2});
  shape_.push_back({-size / 2,  size / 2});
  shape_.push_back({-size / 2, -size / 2});
  shape_.shrink_to_fit();

  render_shape_.resize(shape_.size());
}

void ktp::Aerolite::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
}

void ktp::Aerolite::setScreenSize(const SDL_Point& screen_size) {
  screen_size_ = screen_size;
  screen_size_b2_.x = screen_size.x / kMetersToPixels;
  screen_size_b2_.y = screen_size.y / kMetersToPixels;
}

void ktp::Aerolite::update() {
  transformRenderShape();
  
  if (body_->GetPosition().x < -kMaxSize_ || body_->GetPosition().x > screen_size_b2_.x + kMaxSize_
   || body_->GetPosition().y < -kMaxSize_ || body_->GetPosition().y > screen_size_b2_.y + kMaxSize_) {
    to_be_deleted_ = true;
  }
}