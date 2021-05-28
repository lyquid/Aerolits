#include "aerolite.hpp"
#include "palette.hpp"

ktp::Aerolite::Aerolite(const SDL_FPoint& where) noexcept : center_(where) {
  generateAeroliteShape(kMaxSize_ * generateRand(0.3f, 1.f));

  
  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.position.Set(0.f, 0.f);

  body_ = b2_world_->CreateBody(&body_def);

  b2PolygonShape dynamic_box {};
  dynamic_box.SetAsBox(0.5f, 0.5f);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &dynamic_box;
  fixture_def.density = 1.f;
  fixture_def.friction = 0.3f;

  body_->CreateFixture(&fixture_def);
}

ktp::Aerolite ktp::Aerolite::spawnAerolite(const SDL_Point& screen_size) {
  //Aerolite aerolite {};
  //const auto final_size {kMaxSize_ * generateRand(0.3f, 1.f)};
/* 
  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  // body_def.position.Set(aerolite.position_.x, aerolite.position_.y);

  SpawnSides spawn_side {generateRand(0, 3)};
  //logMessage(std::to_string(spawn_side));
  float x, y;
  switch (spawn_side) {
    case SpawnSides::Top:
      x = static_cast<float>(screen_size.x) * generateRand(0.f, 0.8f);
      y = 0 - final_size;
      body_def.position.Set(x, y);
      aerolite.delta_ = {0, 1};
      break;
    case SpawnSides::Right:
      x = static_cast<float>(screen_size.x) + 1;
      y = static_cast<float>(screen_size.y) * generateRand(0.f, 0.8f);
      body_def.position.Set(x, y);
      aerolite.delta_ = {-1, 0};
      break;
    case SpawnSides::Down:
      x = static_cast<float>(screen_size.x) * generateRand(0.f, 0.8f);
      y = static_cast<float>(screen_size.y) + 1;
      body_def.position.Set(x, y);
      aerolite.delta_ = {0, -1};
      break;
    case SpawnSides::Left:
      x = 0 - final_size;
      y = static_cast<float>(screen_size.y) * generateRand(0.f, 0.8f);
      body_def.position.Set(x, y);
      aerolite.delta_ = {1, 0};
      break;
    default:
      break;
  }
  
  aerolite.body_ = b2_world_->CreateBody(&body_def);

  b2PolygonShape dynamic_box {};
  dynamic_box.SetAsBox((final_size / 20) / 2, (final_size / 20) / 2);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &dynamic_box;
  fixture_def.density = 1.f;
  fixture_def.friction = 0.3f;

  aerolite.body_->CreateFixture(&fixture_def);

  aerolite.shape_.x = aerolite.body_->GetPosition().x;
  aerolite.shape_.y = aerolite.body_->GetPosition().y;
  aerolite.shape_.w = final_size;
  aerolite.shape_.h = final_size; */
  
  //return aerolite;
}

ktp::Aerolite ktp::Aerolite::spawnAerolite2() {
  static int side {};
  constexpr float delta {90};
  Aerolite aerolite({static_cast<float>(screen_size_.x) / 2, static_cast<float>(screen_size_.y) / 2});
  switch (side) {
    case 0: // up
      aerolite.center_ = {static_cast<float>(screen_size_.x) / 2, 0.f};
      aerolite.delta_ = {0, delta};
      ++side;
      break;
    case 1: // right
      aerolite.center_ = {static_cast<float>(screen_size_.x), static_cast<float>(screen_size_.y) / 2};
      aerolite.delta_ = {-delta, 0};
      ++side;
      break;
    case 2: // down
      aerolite.center_ = {static_cast<float>(screen_size_.x) / 2, static_cast<float>(screen_size_.y)};
      aerolite.delta_ = {0, -delta};
      ++side;
      break;
    case 3: // left
      aerolite.center_ = {0.f, static_cast<float>(screen_size_.y) / 2};
      aerolite.delta_ = {delta, 0};
      side = 0;
      break;
  }
  return std::move(aerolite);
}

void ktp::Aerolite::draw(const SDL2_Renderer& renderer) const {
  renderer.setDrawColor(Colors::orange);
  renderer.drawLines(render_shape_);
  renderer.setDrawColor(Colors::yellow);
  renderer.drawPoint(center_);
}

void ktp::Aerolite::generateAeroliteShape(int size) {
  shape_.push_back({static_cast<float>(-size) / 2, static_cast<float>(-size) / 2});
  shape_.push_back({static_cast<float>( size) / 2, static_cast<float>(-size) / 2});
  shape_.push_back({static_cast<float>( size) / 2, static_cast<float>( size) / 2});
  shape_.push_back({static_cast<float>(-size) / 2, static_cast<float>( size) / 2});
  shape_.push_back({static_cast<float>(-size) / 2, static_cast<float>(-size) / 2});
  shape_.shrink_to_fit();

  render_shape_.resize(shape_.size());
}

void ktp::Aerolite::move(float delta_time) {
  center_.x += delta_.x * delta_time;
  center_.y += delta_.y * delta_time;
}

void ktp::Aerolite::rotate() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    render_shape_[i].x = (shape_[i].x * SDL_cosf(angle_) - shape_[i].y * SDL_sinf(angle_)) + center_.x;
    render_shape_[i].y = (shape_[i].x * SDL_sinf(angle_) + shape_[i].y * SDL_cosf(angle_)) + center_.y;
  }
}

void ktp::Aerolite::update(float delta_time) {
  angle_ += rotation_speed_;
  move(delta_time);
  rotate();
  
  if (center_.x < -kMaxSize_ || center_.x > screen_size_.x + kMaxSize_
   || center_.y < -kMaxSize_ || center_.y > screen_size_.y + kMaxSize_) {
    b2_world_->DestroyBody(body_);
    body_ = nullptr;
    to_be_deleted_ = true;
  }

  /* if (body_->GetPosition().x + shape_.w < -threshold || body_->GetPosition().x > screen_size_.x + threshold 
  || -body_->GetPosition().y + shape_.h < -threshold || -body_->GetPosition().y > screen_size_.y + threshold) {
    b2_world_->DestroyBody(body_);
    to_be_deleted_ = true;
  } else {
    shape_.x =  body_->GetPosition().x;
    shape_.y = -body_->GetPosition().y;
  } */
}