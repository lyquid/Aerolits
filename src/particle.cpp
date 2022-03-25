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
bool ktp::Particle::update(float delta_time, GLfloat* subdata) {
  // time step increment to interpolate
  state_.live_.time_step_ += (1.f / state_.live_.start_life_) * delta_time;
  if (state_.live_.time_step_ >= 1.f) state_.live_.time_step_ = 0.f;
  // size interpolation
  if (state_.live_.sizes_.size() == 2) {
    state_.live_.current_size_ = interpolateRange(state_.live_.sizes_[0], state_.live_.sizes_[1], state_.live_.time_step_);
  } else if (state_.live_.sizes_.size() > 2) {
    state_.live_.current_size_ = interpolateRange3(state_.live_.sizes_[0], state_.live_.sizes_[1], state_.live_.sizes_[2], state_.live_.time_step_);
  }
  subdata[7] = state_.live_.current_size_;
  // color interpolation
  if (state_.live_.colors_.size() == 2) {
    state_.live_.current_color_ = interpolate2Colors(state_.live_.colors_[0], state_.live_.colors_[1], state_.live_.time_step_);
  } else if (state_.live_.colors_.size() > 2) {
    state_.live_.current_color_ = interpolate3Colors(state_.live_.colors_[0], state_.live_.colors_[1], state_.live_.colors_[2], state_.live_.time_step_);
  }
  subdata[3] = state_.live_.current_color_.r;
  subdata[4] = state_.live_.current_color_.g;
  subdata[5] = state_.live_.current_color_.b;
  subdata[6] = state_.live_.current_color_.a;
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
  state_.live_.position_.x += state_.live_.current_speed_.x * delta_time;
  state_.live_.position_.y += state_.live_.current_speed_.y * delta_time;
  // translation update
  subdata[0] = state_.live_.position_.x;
  subdata[1] = state_.live_.position_.y;
  subdata[2] = 0.f;

  life_ -= delta_time;
  return life_ <= 0.f;
}
