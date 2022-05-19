#include "include/emitter.hpp"
#include "include/particle.hpp"

void ktp::Particle::init(const ParticleData& data) {
  start_life_             = data.start_life_;
  current_life_           = data.start_life_;
  colors_                 = data.colors_;
  current_color_          = data.current_color_;
  sizes_                  = data.sizes_;
  current_size_           = data.current_size_;
  speeds_                 = data.speeds_;
  current_speed_          = data.current_speed_;
  rotation_               = data.rotation_;
  start_rotation_speed_   = data.start_rotation_speed_;
  current_rotation_speed_ = data.current_rotation_speed_;
  end_rotation_speed_     = data.end_rotation_speed_;
  position_               = data.position_;
  time_step_              = data.time_step_;
}

// returns true if the particle has died in this frame
bool ktp::Particle::update(float delta_time, GLfloat* subdata) {
  // time step increment to interpolate
  time_step_ += (1.f / start_life_) * delta_time;
  if (time_step_ >= 1.f) time_step_ = 0.f;
  // size interpolation
  if (sizes_.size() == 2) {
    current_size_ = Palette::interpolateRange(sizes_[0], sizes_[1], time_step_);
  } else if (sizes_.size() > 2) {
    current_size_ = Palette::interpolateRange3(sizes_[0], sizes_[1], sizes_[2], time_step_);
  }
  subdata[7] = current_size_;
  // color interpolation
  if (colors_.size() == 2) {
    current_color_ = Palette::interpolate2Colors(colors_[0], colors_[1], time_step_);
  } else if (colors_.size() > 2) {
    current_color_ = Palette::interpolate3Colors(colors_[0], colors_[1], colors_[2], time_step_);
  }
  subdata[3] = current_color_.r;
  subdata[4] = current_color_.g;
  subdata[5] = current_color_.b;
  subdata[6] = current_color_.a;
  // rotation speed interpolation
  current_rotation_speed_ = Palette::interpolateRange(start_rotation_speed_, end_rotation_speed_, time_step_);
  rotation_ += current_rotation_speed_;
  // speed interpolation
  if (speeds_.size() == 2) {
    current_speed_.x = Palette::interpolateRange(speeds_[0].x, speeds_[1].x, time_step_);
    current_speed_.y = Palette::interpolateRange(speeds_[0].y, speeds_[1].y, time_step_);
  } else if (speeds_.size() > 2) {
    current_speed_.x = Palette::interpolateRange3(speeds_[0].x, speeds_[1].x, speeds_[2].x, time_step_);
    current_speed_.y = Palette::interpolateRange3(speeds_[0].y, speeds_[1].y, speeds_[2].y, time_step_);
  }
  // position update
  position_.x += current_speed_.x * delta_time;
  position_.y += current_speed_.y * delta_time;
  // translation update
  subdata[0] = position_.x;
  subdata[1] = position_.y;
  subdata[2] = 0.f;

  current_life_ -= delta_time;
  return current_life_ <= 0.f;
}
