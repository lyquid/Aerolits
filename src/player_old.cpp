#include "include/box2d_scale.hpp"
#include "include/player.hpp"

ktp::Player::Player(SDL_Point& screen_size, kuge::EventBus& event_bus, b2World* world, InputComponents input):
  event_bus_(event_bus),
  screen_size_b2_({(float)screen_size.x / kMetersToPixels, (float)screen_size.y / kMetersToPixels}),
  world_(world) {

  switch (input) {
    case InputComponents::Demo:
      input_ = std::make_unique<DemoInputComponent>();
      break;
    case InputComponents::Player:
      [[fallthrought]]
    default:
      input_ = std::make_unique<PlayerInputComponent>();
      break;
  }

  graphics_ = std::make_unique<PlayerGraphicsComponent>();

  generatePlayerShape();
  generateLaserShape();

  lasers_.reserve(50);

  setBox2D();
}

void ktp::Player::checkWrap() {
  const auto threshold {size_ * 0.5f};
  if (body_->GetPosition().x < -threshold) {
    body_->SetTransform({screen_size_b2_.x + threshold, body_->GetPosition().y}, body_->GetAngle());
  } else if (body_->GetPosition().x > screen_size_b2_.x + threshold) {
    body_->SetTransform({-threshold, body_->GetPosition().y}, body_->GetAngle());
  }
  if (body_->GetPosition().y < -threshold) {
    body_->SetTransform({body_->GetPosition().x, screen_size_b2_.y + threshold}, body_->GetAngle());
  } else if (body_->GetPosition().y > screen_size_b2_.y + threshold) {
    body_->SetTransform({body_->GetPosition().x, -threshold}, body_->GetAngle());
  }
}

void ktp::Player::draw(SDL2_Renderer& renderer) const {
  graphics_->update(*this, renderer);
}

void ktp::Player::generateLaserShape() {
  laser_shape_.clear();
  laser_shape_.push_back({-kDefaultLaserSize_ * 0.15f, -kDefaultLaserSize_}); // top left
  laser_shape_.push_back({-kDefaultLaserSize_ * 0.15f,  kDefaultLaserSize_}); // down left
  laser_shape_.push_back({ kDefaultLaserSize_ * 0.15f,  kDefaultLaserSize_}); // down right
  laser_shape_.push_back({ kDefaultLaserSize_ * 0.15f, -kDefaultLaserSize_}); // top right
  laser_shape_.push_back({-kDefaultLaserSize_ * 0.15f, -kDefaultLaserSize_}); // top left again
  laser_shape_.shrink_to_fit();
}

void ktp::Player::generatePlayerShape() {
  shape_.clear();
  shape_.push_back({           0.f, -size_ * 0.50f}); // top
  shape_.push_back({-size_ * 0.33f,  size_ * 0.50f}); // left
  shape_.push_back({-size_ * 0.15f,  size_ * 0.33f}); // left flap
  shape_.push_back({ size_ * 0.15f,  size_ * 0.33f}); // right flap
  shape_.push_back({ size_ * 0.33f,  size_ * 0.50f}); // right
  shape_.push_back({           0.f, -size_ * 0.50f}); // top again
  shape_.shrink_to_fit();
  render_shape_.resize(shape_.size());

  flame_shape_.clear();
  flame_shape_.push_back({           0.f, size_ * 0.40f}); // bottom           2 ____ 1
  flame_shape_.push_back({ size_ * 0.15f, size_ * 0.35f}); // right vertice      \  /
  flame_shape_.push_back({-size_ * 0.15f, size_ * 0.35f}); // left vertice        \/
  flame_shape_.push_back({           0.f, size_ * 0.40f}); // bottom again        3/0
  flame_shape_.shrink_to_fit();
  render_flame_shape_.resize(flame_shape_.size());
}

void ktp::Player::reset() {
  alive_ = true;
  delta_ = {0, 0};
  size_ = kDefaultPlayerSize_;
  thrusting_ = false;
  /* components */
  // input_->reset();
  /* shooting stuff */
  lasers_.clear();
  /* thrust and flame stuff */
  flame_growth_factor_ = kDefaultFlameGrowthFactor_;
  flame_max_lenght_ = kDefaultFlameMaxLength_;
}

void ktp::Player::setBox2D() {
  /* Player */
  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.bullet = true;
  body_def.position.Set(screen_size_b2_.x * 0.5f, screen_size_b2_.y * 0.5f);
  body_def.userData.pointer = reinterpret_cast<uintptr_t>(this);

  body_ = world_->CreateBody(&body_def);
  // triangle CCW
  b2Vec2 vertices[3];
  vertices[0].Set(           0.f, -size_ * 0.50f); // top
  vertices[1].Set(-size_ * 0.33f,  size_ * 0.50f); // left
  vertices[2].Set( size_ * 0.33f,  size_ * 0.50f); // right
  b2PolygonShape triangle {};
  triangle.Set(vertices, 3);

  b2FixtureDef fixture_def {};
  fixture_def.shape = &triangle;
  fixture_def.density = 1.5f;
  fixture_def.friction = 0.8f;
  // https://www.iforce2d.net/b2dtut/collision-filtering
  fixture_def.filter.groupIndex = -1;

  body_->CreateFixture(&fixture_def);

  /* Lasers */
  laser_body_def_.type = b2_dynamicBody;
  laser_body_def_.bullet = true;
  
  b2Vec2 laser_vertices[4];
  laser_vertices[0].Set(-kDefaultLaserSize_ * 0.15f, -kDefaultLaserSize_); // top left
  laser_vertices[1].Set(-kDefaultLaserSize_ * 0.15f,  kDefaultLaserSize_); // down left
  laser_vertices[2].Set( kDefaultLaserSize_ * 0.15f,  kDefaultLaserSize_); // down right
  laser_vertices[3].Set( kDefaultLaserSize_ * 0.15f, -kDefaultLaserSize_); // up right
  laser_shape_b2_.Set(laser_vertices, 4);

  laser_fixture_def_.shape = &laser_shape_b2_;
  laser_fixture_def_.density = 10.f;
  laser_fixture_def_.friction = 0.1f;
  laser_fixture_def_.filter.groupIndex = -1;
  laser_fixture_def_.restitution = 0.35f;
}

void ktp::Player::transformRenderShape() {
  for (auto i = 0u; i < shape_.size(); ++i) {
    render_shape_[i].x = ((shape_[i].x * SDL_cosf(body_->GetAngle()) - shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
    render_shape_[i].y = ((shape_[i].x * SDL_sinf(body_->GetAngle()) + shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
  }
  if (thrusting_) {
    for (auto i = 0u; i < flame_shape_.size(); ++i) {
      render_flame_shape_[i].x = ((flame_shape_[i].x * SDL_cosf(body_->GetAngle()) - flame_shape_[i].y * SDL_sinf(body_->GetAngle())) + body_->GetPosition().x) * kMetersToPixels;
      render_flame_shape_[i].y = ((flame_shape_[i].x * SDL_sinf(body_->GetAngle()) + flame_shape_[i].y * SDL_cosf(body_->GetAngle())) + body_->GetPosition().y) * kMetersToPixels;
    }
  }
}

void ktp::Player::update(float delta_time) {
  input_->update(*this, delta_time);

  checkWrap();

  transformRenderShape();
  
  exhaust_emitter_.setPosition({body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels});
  exhaust_emitter_.update();

  if (!lasers_.empty()) updateLasers();
}

void ktp::Player::updateLasers() {
  constexpr auto threshold {kDefaultLaserSize_};
  auto laser = lasers_.begin();
  while (laser != lasers_.end()) {
    // check if laser is out of screen
    if (laser->body_->GetPosition().x < -threshold || laser->body_->GetPosition().x > screen_size_b2_.x + threshold ||
        laser->body_->GetPosition().y < -threshold || laser->body_->GetPosition().y > screen_size_b2_.y + threshold) {
      world_->DestroyBody(laser->body_);
      laser = lasers_.erase(laser);
    } else {
      // move the laser's shape
      for (auto i = 0u; i < laser_shape_.size(); ++i) {
        laser->render_shape_[i].x = ((laser_shape_[i].x * SDL_cosf(laser->body_->GetAngle()) - laser_shape_[i].y * SDL_sinf(laser->body_->GetAngle())) + laser->body_->GetPosition().x) * kMetersToPixels;
        laser->render_shape_[i].y = ((laser_shape_[i].x * SDL_sinf(laser->body_->GetAngle()) + laser_shape_[i].y * SDL_cosf(laser->body_->GetAngle())) + laser->body_->GetPosition().y) * kMetersToPixels;
      }
      ++laser;
    }
  }
}