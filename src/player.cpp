#include "player.hpp"
#include "scale.hpp"

ktp::Player::Player(SDL_Point& screen_size, kuge::EventBus& event_bus):
  event_bus_(event_bus),
  screen_size_b2_({screen_size.x / kMetersToPixels, screen_size.y / kMetersToPixels}) {

  generatePlayerShape();
  
  lasers_.reserve(50);
}

void ktp::Player::checkWrap() {
  const auto threshold {size_ * 0.5f};
  
  if (body_->GetPosition().x < -threshold) {
    body_->SetTransform({screen_size_b2_.x + threshold, body_->GetPosition().y}, body_->GetAngle());
  } else  if (body_->GetPosition().x > screen_size_b2_.x + threshold) {
    body_->SetTransform({-threshold, body_->GetPosition().y}, body_->GetAngle());
  }

  if (body_->GetPosition().y < -threshold) {
    body_->SetTransform({body_->GetPosition().x, screen_size_b2_.y + threshold}, body_->GetAngle());
  } else  if (body_->GetPosition().y > screen_size_b2_.y + threshold) {
    body_->SetTransform({body_->GetPosition().x, -threshold}, body_->GetAngle());
  }
}

void ktp::Player::draw(SDL2_Renderer& renderer) const {
  /* player's shape */
  renderer.setDrawColor(kDefaultPlayerColor_);
  renderer.drawLines(render_shape_);
  /* player's thrust fx */
  if (thrusting_) {
    renderer.setDrawColor(Colors::red);
    renderer.drawLines(render_flame_shape_);
  }
  /* particles */
  exhaust_emitter_.draw();
  /* lasers */
  renderer.setDrawColor(Colors::orange);
  for (const auto& laser: lasers_) {
    renderer.drawLines(laser.shape_);
  }
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
  angular_impulse_ = kDefaultAngularImpulse_;
  alive_ = true;
  delta_ = {0, 0};
  linear_impulse_ = kDefaultLinearImpulse_;
  size_ = kDefaultPlayerSize_;
  thrusting_ = false;
  /* shooting stuff */
  lasers_.clear();
  shooting_timer_ = 0.f;
  /* thrust and flame stuff */
  flame_growth_factor_ = kDefaultFlameGrowthFactor_;
  flame_max_lenght_ = kDefaultFlameMaxLength_;

  //update(0.f); // needed to initialize render_shape_ // ????
}

void ktp::Player::setBox2d(b2World* world) {
  world_ = world;

  b2BodyDef body_def {};
  body_def.type = b2_dynamicBody;
  body_def.bullet = true;
  body_def.position.Set(screen_size_b2_.x / 2, screen_size_b2_.y / 2);
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

  body_->CreateFixture(&fixture_def);
}

void ktp::Player::shoot() {
  if ((SDL_GetTicks() - shooting_timer_) > kDefaultShootingInterval_) {
    Laser laser;
    laser.angle_ = body_->GetAngle();
    laser.delta_.x =  kDefaultLaserSpeed_ * SDL_sinf(body_->GetAngle());
    laser.delta_.y = -kDefaultLaserSpeed_ * SDL_cosf(body_->GetAngle());
    laser.shape_.resize(2);
    // laser head
    laser.shape_[0].x = render_shape_.front().x;
    laser.shape_[0].y = render_shape_.front().y;
    // laser tail
    laser.shape_[1].x = -laser.size_ * SDL_sinf(laser.angle_) + render_shape_.front().x;
    laser.shape_[1].y =  laser.size_ * SDL_cosf(laser.angle_) + render_shape_.front().y;
    
    lasers_.push_back(laser);
    shooting_timer_ = SDL_GetTicks();

    //event_bus_.postEvent(kuge::EventTypes::LaserFired);
  }
}

void ktp::Player::steerLeft () {
  body_->SetAngularVelocity(-angular_impulse_ );
  stabilizer_time_ = SDL2_Timer::getSDL2Ticks();
  steering_ = true;
}

void ktp::Player::steerRight () {
  body_->SetAngularVelocity(angular_impulse_ );
  stabilizer_time_ = SDL2_Timer::getSDL2Ticks();
  steering_ = true;
}

void ktp::Player::stopThrusting() {
  thrusting_ = false;
  delta_ = {0.f, 0.f};
  flame_shape_.front().y = kDefaultFlameMinLength_;
  flame_shape_.back().y = kDefaultFlameMinLength_;
}

void ktp::Player::thrust(float delta_time) {
  delta_.x +=  SDL_sinf(body_->GetAngle()) * linear_impulse_ * delta_time;
  if (delta_.x < -kMaxDelta_ ) {
    delta_.x = -kMaxDelta_;
  } else if (delta_.x > kMaxDelta_) {
    delta_.x = kMaxDelta_;
  }
  delta_.y += -SDL_cosf(body_->GetAngle()) * linear_impulse_ * delta_time;
  if (delta_.y < -kMaxDelta_) {
    delta_.y = -kMaxDelta_;
  } else if (delta_.y > kMaxDelta_) {
    delta_.y = kMaxDelta_;
  }

  body_->ApplyLinearImpulseToCenter(delta_, true);
  
  thrusting_ = true;
  
  if (flame_shape_.front().y < flame_max_lenght_) {
    flame_shape_.front().y += flame_growth_factor_;
    flame_shape_.back().y += flame_growth_factor_;
  }

  // exhaust_emitter_.generateParticles();
  event_bus_.postEvent(kuge::EventTypes::PlayerThrust);
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
  if (SDL2_Timer::getSDL2Ticks() - stabilizer_time_ > delta_time && steering_) {
    body_->SetAngularVelocity(0.f);
    steering_ = false;
  }

  checkWrap();
  transformRenderShape();

  exhaust_emitter_.setPosition({body_->GetPosition().x * kMetersToPixels, body_->GetPosition().y * kMetersToPixels});
  exhaust_emitter_.update();

  if (!lasers_.empty()) updateLasers(delta_time);
}

void ktp::Player::updateLasers(float delta_time) {
  constexpr auto threshold = 100.f;
  auto laser = lasers_.begin();
  while (laser != lasers_.end()) {
    // check if laser is out of screen
    if (laser->shape_.front().x < -threshold || laser->shape_.front().x > screen_size_b2_.x + threshold ||
        laser->shape_.front().y < -threshold || laser->shape_.front().y > screen_size_b2_.y + threshold) {

      laser = lasers_.erase(laser);
    } else {
      // move the laser's shape
      for (auto& point: laser->shape_) {
        point.x += laser->delta_.x * delta_time;
        point.y += laser->delta_.y * delta_time;
      }
      ++laser;
    }
  }
}