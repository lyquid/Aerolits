#include "player.h"

ktp::Player::Player(SDL_Point& screen_size, kuge::EventBus& event_bus):
  event_bus_(event_bus),
  screen_size_(screen_size),
  center_({screen_size.x / 2.f, screen_size.y / 2.f}) {

  generatePlayerShape();
  render_shape_.resize(shape_.size());
  render_flame_shape_.resize(flame_shape_.size());
  update(0.f); // needed to initialize render_shape_
  lasers_.reserve(50);
}

void ktp::Player::draw(SDL2_Renderer& renderer) const {
  /* player's shape */
  renderer.setDrawColor(kDefaultPlayerColor_);
  renderer.drawLines(render_shape_);
  /* player's thrust fx */
  if (thrusting_) {
    renderer.setDrawColor(ktp::Colors::red);
    renderer.drawLines(render_flame_shape_);
  }
  /* particles */
  particle_pool_.draw(renderer);
  /* lasers */
  renderer.setDrawColor(ktp::Colors::orange);
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

  flame_shape_.clear();
  flame_shape_.push_back({           0.f, size_ * 0.40f}); // bottom           2 ____ 1
  flame_shape_.push_back({ size_ * 0.15f, size_ * 0.35f}); // right vertice      \  /
  flame_shape_.push_back({-size_ * 0.15f, size_ * 0.35f}); // left vertice        \/
  flame_shape_.push_back({           0.f, size_ * 0.40f}); // bottom again        3/0
  flame_shape_.shrink_to_fit();
}

void ktp::Player::move(float delta_time) {
  const auto threshold = size_ * 0.5f;
  center_.x += delta_.x * delta_time;
  if (center_.x < -threshold) {
    center_.x = screen_size_.x + threshold;
  } else  if (center_.x > screen_size_.x + threshold) {
    center_.x = -threshold;
  }

  center_.y += delta_.y * delta_time;
  if (center_.y < -threshold) {
    center_.y = screen_size_.y + threshold;
  } else  if (center_.y > screen_size_.y + threshold) {
    center_.y = -threshold;
  }
}

void ktp::Player::reset() {
  alive_ = true;
  angle_ = 0.f;
  center_ = {screen_size_.x / 2.f, screen_size_.y / 2.f};
  delta_ = {0, 0};
  /* shooting stuff */
  lasers_.clear();
  shooting_timer_ = 0.f;
  /* thrust and flame stuff */
  flame_growth_factor_ = kDefaultFlameGrowthFactor_;
  flame_max_lenght_ = kDefaultFlameMaxLength_;
  thrust_speed_ = kDefaultThrustSpeed_;
  thrusting_ = false;

  update(0.f); // needed to initialize render_shape_
}

void ktp::Player::rotate() {
  // adding the center at the end makes the shape move to the correct position
  for (auto i = 0u; i < shape_.size(); ++i) {
    render_shape_[i].x = (shape_[i].x * SDL_cosf(angle_) - shape_[i].y * SDL_sinf(angle_)) + center_.x;
    render_shape_[i].y = (shape_[i].x * SDL_sinf(angle_) + shape_[i].y * SDL_cosf(angle_)) + center_.y;
  }
  if (thrusting_) { // this makes the first particles to appear in a previous (wrong) position
    for (auto i = 0u; i < flame_shape_.size(); ++i) {
      render_flame_shape_[i].x = (flame_shape_[i].x * SDL_cosf(angle_) - flame_shape_[i].y * SDL_sinf(angle_)) + center_.x;
      render_flame_shape_[i].y = (flame_shape_[i].x * SDL_sinf(angle_) + flame_shape_[i].y * SDL_cosf(angle_)) + center_.y;
    }
  }
}

void ktp::Player::shoot() {
  if ((SDL_GetTicks() - shooting_timer_) > kDefaultShootingInterval_) {
    Laser laser;
    laser.angle_ = angle_;
    laser.delta_.x =  kDefaultLaserSpeed_ * SDL_sinf(angle_);
    laser.delta_.y = -kDefaultLaserSpeed_ * SDL_cosf(angle_);
    laser.shape_.resize(2);
    // laser head
    laser.shape_[0].x = render_shape_.front().x;
    laser.shape_[0].y = render_shape_.front().y;
    // laser tail
    laser.shape_[1].x = -laser.size_ * SDL_sinf(laser.angle_) + render_shape_.front().x;
    laser.shape_[1].y =  laser.size_ * SDL_cosf(laser.angle_) + render_shape_.front().y;
    
    lasers_.push_back(laser);
    shooting_timer_ = SDL_GetTicks();
    event_bus_.postEvent(kuge::EventTypes::LaserFired);
  }
}

void ktp::Player::stopThrusting() {
  thrusting_ = false;
  flame_shape_.front().y = kDefaultFlameMinLength_;
  flame_shape_.back().y = kDefaultFlameMinLength_;
}

void ktp::Player::thrust(float delta_time) {
  delta_.x +=  SDL_sinf(angle_) * thrust_speed_ * delta_time;
  if (delta_.x < -kMaxDelta_ ) {
    delta_.x = -kMaxDelta_;
  } else if (delta_.x > kMaxDelta_) {
    delta_.x = kMaxDelta_;
  }
  delta_.y += -SDL_cosf(angle_) * thrust_speed_ * delta_time;
  if (delta_.y < -kMaxDelta_) {
    delta_.y = -kMaxDelta_;
  } else if (delta_.y > kMaxDelta_) {
    delta_.y = kMaxDelta_;
  }
  thrusting_ = true;
  if (flame_shape_.front().y < flame_max_lenght_) {
    flame_shape_.front().y += flame_growth_factor_;
    flame_shape_.back().y += flame_growth_factor_;
  }

  constexpr float max_delta = 200.f;
  const SDL_FPoint inverse_delta{delta_.x * -0.5f, delta_.y * -0.5f};
  particle_pool_.generate(render_flame_shape_[1], inverse_delta, 200);
  particle_pool_.generate(render_flame_shape_.front(), inverse_delta, 200);
  particle_pool_.generate(render_flame_shape_[2], inverse_delta, 200);
  event_bus_.postEvent(kuge::EventTypes::PlayerThrust);
}

void ktp::Player::update(float delta_time) {
  move(delta_time);
  rotate();
  particle_pool_.update(delta_time);
  if (!lasers_.empty()) updateLasers(delta_time);
}

void ktp::Player::updateLasers(float delta_time) {
  constexpr auto threshold = 100.f;
  auto laser = lasers_.begin();
  while (laser != lasers_.end()) {
    // check if laser is out of screen
    if (laser->shape_.front().x < -threshold || laser->shape_.front().x > screen_size_.x + threshold ||
        laser->shape_.front().y < -threshold || laser->shape_.front().y > screen_size_.y + threshold) {

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