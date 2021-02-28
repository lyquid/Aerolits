#include "player.h"

ktp::Player::Player(SDL_Point& screen_size, kuge::EventBus& event_bus):
  event_bus_(event_bus),
  screen_size_(screen_size),
  center_({screen_size.x / 2.f, screen_size.y / 2.f}) {

  generatePlayerShape();
  render_shape_.resize(shape_.size());
  update(0.f); // needed to initialize render_shape_
  lasers_.reserve(50);
}

void ktp::Player::draw(SDL2_Renderer& renderer) {
  /* player's shape */
  renderer.setDrawColor(kDefaultPlayerColor_);
  renderer.drawLines(render_shape_);
  /* lasers */
  renderer.setDrawColor(255, 104, 10, 255);
  for (auto& laser: lasers_) {
    renderer.drawLines(laser.shape_);
  }
}

void ktp::Player::generatePlayerShape() {
  shape_.clear();
  shape_.push_back({             0, -size_ * 0.50f}); // top
  shape_.push_back({-size_ * 0.33f,  size_ * 0.50f}); // left
  shape_.push_back({-size_ * 0.15f,  size_ * 0.33f}); // left flap
  shape_.push_back({ size_ * 0.15f,  size_ * 0.33f}); // right flap
  shape_.push_back({ size_ * 0.33f,  size_ * 0.50f}); // right
  shape_.push_back({             0, -size_ * 0.50f}); // top again 
}

void ktp::Player::move(float delta_time) {
  center_.x += delta_.x * delta_time;
  if (center_.x < -size_ * 0.5f) {
    center_.x = screen_size_.x + (size_ * 0.5f);
  } else  if (center_.x > screen_size_.x + (size_ * 0.5f)) {
    center_.x = -size_ * 0.5f;
  }

  center_.y += delta_.y * delta_time;
  if (center_.y < -size_ * 0.5f) {
    center_.y = screen_size_.y + (size_ * 0.5f);
  } else  if (center_.y > screen_size_.y + (size_ * 0.5f)) {
    center_.y = -size_ * 0.5f;
  }
}

void ktp::Player::reset() {
  alive_ = true;
  angle_ = 0.f;
  center_ = {screen_size_.x / 2.f, screen_size_.y / 2.f};
  delta_ = {0, 0};
  lasers_.clear();
  shooting_timer_ = 0.f;
  update(0.f); // needed to initialize render_shape_
}

void ktp::Player::rotate() {
  // adding the center ath the end makes the shape move to the correct position
  for (auto i = 0u; i < shape_.size(); ++i) {
    render_shape_[i].x = (shape_[i].x * SDL_cosf(angle_) - shape_[i].y * SDL_sinf(angle_)) + center_.x;
    render_shape_[i].y = (shape_[i].x * SDL_sinf(angle_) + shape_[i].y * SDL_cosf(angle_)) + center_.y;
  }
}

void ktp::Player::shoot() {
  if ((SDL_GetTicks() - shooting_timer_) > 200.f) {
    Laser laser;
    laser.angle_ = angle_;
    laser.delta_.x =  800.0f * SDL_sinf(angle_);
    laser.delta_.y = -800.0f * SDL_cosf(angle_);

    laser.shape_.resize(2);
    const SDL_FPoint head = {0, 0};
    const SDL_FPoint tail = {0, laser.size_};
    laser.shape_[0].x = (head.x * SDL_cosf(laser.angle_) - head.y * SDL_sinf(laser.angle_)) + render_shape_.front().x;
    laser.shape_[0].y = (head.x * SDL_sinf(laser.angle_) + head.y * SDL_cosf(laser.angle_)) + render_shape_.front().y;

    laser.shape_[1].x = (tail.x * SDL_cosf(laser.angle_) - tail.y * SDL_sinf(laser.angle_)) + render_shape_.front().x;
    laser.shape_[1].y = (tail.x * SDL_sinf(laser.angle_) + tail.y * SDL_cosf(laser.angle_)) + render_shape_.front().y;

    lasers_.push_back(laser);
    shooting_timer_ = SDL_GetTicks();
    event_bus_.postEvent(kuge::EventTypes::LaserFired);
  }
}

void ktp::Player::thrust(float delta_time) {
  delta_.x +=  SDL_sinf(angle_) * 200.f * delta_time;
  if (delta_.x < -kMaxDelta_ ) {
    delta_.x = -kMaxDelta_;
  } else if (delta_.x > kMaxDelta_) {
    delta_.x = kMaxDelta_;
  }
  delta_.y += -SDL_cosf(angle_) * 200.f * delta_time;
  if (delta_.y < -kMaxDelta_) {
    delta_.y = -kMaxDelta_;
  } else if (delta_.y > kMaxDelta_) {
    delta_.y = kMaxDelta_;
  }
}

void ktp::Player::update(float delta_time) {
  rotate();
  move(delta_time);
  // alive_ = !checkPlayerCollisions(aerolites);
  if (!lasers_.empty()) updateLasers(delta_time);
}

void ktp::Player::updateLasers(float delta_time) {
  auto laser = lasers_.begin();
  while (laser != lasers_.end()) {
    if (laser->shape_.front().x < -100 || laser->shape_.front().x > screen_size_.x + 100 ||
        laser->shape_.front().y < -100 || laser->shape_.front().y > screen_size_.y + 100) {

      laser = lasers_.erase(laser);
    } else {
      for (auto& point: laser->shape_) {
        point.x += laser->delta_.x * delta_time;
        point.y += laser->delta_.y * delta_time;
      }
      ++laser;
    }
  }
}
