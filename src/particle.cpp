#include "include/emitter.hpp" // Vortex
#include "include/particle.hpp"

ktp::Particle& ktp::Particle::operator=(const Particle& other) {
  life_  = other.life_;
  state_ = other.state_;
  return *this;
}

ktp::Particle& ktp::Particle::operator=(Particle&& other) {
  if (this != &other) {
    life_  = other.life_;
    state_ = std::move(other.state_);
  }
  return *this;
}

ktp::Particle::State& ktp::Particle::State::operator=(const State& other) {
  live_ = other.live_;
  next_ = other.next_;
  return *this;
}

ktp::Particle::State& ktp::Particle::State::operator=(State&& other) {
  if (this != &other) {
    live_ = std::move(other.live_);
    next_ = std::exchange(other.next_, nullptr);
  }
  return *this;
}

void ktp::Particle::init(const ParticleData& data) {
  life_ = data.start_life_;
  state_.live_ = data;
  // position is set to center the texture on the emitter
  // state_.live_.position_ = {data.position_.x - data.current_size_ * 0.5f,
  //                           data.position_.y - data.current_size_ * 0.5f};
}

glm::vec4 ktp::Particle::interpolate2Colors(const glm::vec4& start_color, const glm::vec4& end_color, float time_step) {
  return {
    interpolateRange(start_color.r, end_color.r, time_step),
    interpolateRange(start_color.g, end_color.g, time_step),
    interpolateRange(start_color.b, end_color.b, time_step),
    interpolateRange(start_color.a, end_color.a, time_step)
  };
}

glm::vec4 ktp::Particle::interpolate3Colors(const glm::vec4& start_color, const glm::vec4& mid_color, const glm::vec4& end_color, float time_step) {
  return {
    interpolateRange3(start_color.r, mid_color.r, end_color.r, time_step),
    interpolateRange3(start_color.g, mid_color.g, end_color.g, time_step),
    interpolateRange3(start_color.b, mid_color.b, end_color.b, time_step),
    interpolateRange3(start_color.a, mid_color.a, end_color.a, time_step)
  };
}

/* return true if the previously live particle gave up the ghost in that frame */
bool ktp::Particle::update(glm::vec3& pos, glm::vec4& color) {
  // time step increment to interpolate
  state_.live_.time_step_ += (1.f / state_.live_.start_life_);
  if (state_.live_.time_step_ >= 1.f) state_.live_.time_step_ = 0.f;
  // size interpolation
  const float last_size {state_.live_.current_size_};
  if (state_.live_.sizes_.size() == 2) {
    state_.live_.current_size_ = interpolateRange(state_.live_.sizes_[0], state_.live_.sizes_[1], state_.live_.time_step_);
  } else if (state_.live_.sizes_.size() > 2) {
    state_.live_.current_size_ = interpolateRange3(state_.live_.sizes_[0], state_.live_.sizes_[1], state_.live_.sizes_[2], state_.live_.time_step_);
  }
  // new position based on the new size
  state_.live_.position_.x += ((last_size - state_.live_.current_size_) * 0.5f);
  state_.live_.position_.y += ((last_size - state_.live_.current_size_) * 0.5f);
  // color interpolation
  if (state_.live_.colors_.size() == 2) {
    state_.live_.current_color_ = interpolate2Colors(state_.live_.colors_[0], state_.live_.colors_[1], state_.live_.time_step_);
    color = state_.live_.current_color_;
  } else if (state_.live_.colors_.size() > 2) {
    state_.live_.current_color_ = interpolate3Colors(state_.live_.colors_[0], state_.live_.colors_[1], state_.live_.colors_[2], state_.live_.time_step_);
    color = state_.live_.current_color_;
  }
  // rotation speed interpolation
  state_.live_.current_rotation_speed_ = interpolateRange(state_.live_.start_rotation_speed_, state_.live_.end_rotation_speed_, state_.live_.time_step_);
  state_.live_.rotation_ += state_.live_.current_rotation_speed_;
  // speed interpolation
  if (state_.live_.speeds_.size() == 2) {
    state_.live_.current_speed_.x = interpolateRange(state_.live_.speeds_[0].x, state_.live_.speeds_[1].x, state_.live_.time_step_);
    state_.live_.current_speed_.y = interpolateRange(state_.live_.speeds_[0].y, state_.live_.speeds_[1].y, state_.live_.time_step_);
  } else if (state_.live_.speeds_.size() > 2) {
    state_.live_.current_speed_.x = interpolateRange3(state_.live_.speeds_[0].x, state_.live_.speeds_[1].x, state_.live_.speeds_[2].x, state_.live_.time_step_);
    state_.live_.current_speed_.y = interpolateRange3(state_.live_.speeds_[0].y, state_.live_.speeds_[1].y, state_.live_.speeds_[2].y, state_.live_.time_step_);
  }
  // position update
  state_.live_.position_.x += state_.live_.current_speed_.x;
  state_.live_.position_.y += state_.live_.current_speed_.y;
  // translation update
  pos.x = state_.live_.position_.x;
  pos.y = state_.live_.position_.y;

  --life_;
  return life_ == 0;
}

/* return true if the previously live particle gave up the ghost in that frame */
bool ktp::Particle::update(const Vortex& vortex, glm::vec3& pos, glm::vec4& color) {
  // time step increment to interpolate
  state_.live_.time_step_ += (1.f / state_.live_.start_life_);
  if (state_.live_.time_step_ >= 1.f) state_.live_.time_step_ = 0.f;
  // size interpolation
  const float last_size {state_.live_.current_size_};
  if (state_.live_.sizes_.size() == 2) {
    state_.live_.current_size_ = interpolateRange(state_.live_.sizes_[0], state_.live_.sizes_[1], state_.live_.time_step_);
  } else if (state_.live_.sizes_.size() > 2) {
    state_.live_.current_size_ = interpolateRange3(state_.live_.sizes_[0], state_.live_.sizes_[1], state_.live_.sizes_[2], state_.live_.time_step_);
  }
  // new position based on the new size
  state_.live_.position_.x += ((last_size - state_.live_.current_size_) * 0.5f);
  state_.live_.position_.y += ((last_size - state_.live_.current_size_) * 0.5f);
  // color interpolation
  if (state_.live_.colors_.size() == 2) {
    state_.live_.current_color_ = interpolate2Colors(state_.live_.colors_[0], state_.live_.colors_[1], state_.live_.time_step_);
    color = state_.live_.current_color_;
  } else if (state_.live_.colors_.size() > 2) {
    state_.live_.current_color_ = interpolate3Colors(state_.live_.colors_[0], state_.live_.colors_[1], state_.live_.colors_[2], state_.live_.time_step_);
    color = state_.live_.current_color_;
  }
  // rotation speed interpolation
  state_.live_.current_rotation_speed_ = interpolateRange(state_.live_.start_rotation_speed_, state_.live_.end_rotation_speed_, state_.live_.time_step_);
  state_.live_.rotation_ += state_.live_.current_rotation_speed_;
  // speed interpolation
  if (state_.live_.speeds_.size() == 2) {
    state_.live_.current_speed_.x = interpolateRange(state_.live_.speeds_[0].x, state_.live_.speeds_[1].x, state_.live_.time_step_);
    state_.live_.current_speed_.y = interpolateRange(state_.live_.speeds_[0].y, state_.live_.speeds_[1].y, state_.live_.time_step_);
  } else if (state_.live_.speeds_.size() > 2) {
    state_.live_.current_speed_.x = interpolateRange3(state_.live_.speeds_[0].x, state_.live_.speeds_[1].x, state_.live_.speeds_[2].x, state_.live_.time_step_);
    state_.live_.current_speed_.y = interpolateRange3(state_.live_.speeds_[0].y, state_.live_.speeds_[1].y, state_.live_.speeds_[2].y, state_.live_.time_step_);
  }
  // vortex stuff
  const auto dx {state_.live_.position_.x - vortex.position_.x};
  const auto dy {state_.live_.position_.y - vortex.position_.y};

  const auto vx {-dy * vortex.speed_};
  const auto vy { dx * vortex.speed_};

  const auto factor {1.f / (1.f + (dx * dx + dy * dy) / vortex.scale_)};
  // position update
  state_.live_.position_.x += (vx - state_.live_.current_speed_.x) * factor;
  state_.live_.position_.y += (vy - state_.live_.current_speed_.y) * factor;
  // translation update
  pos.x = state_.live_.position_.x;
  pos.y = state_.live_.position_.y;

  --life_;
  return life_ == 0;
}
