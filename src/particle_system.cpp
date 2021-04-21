#include "palette.hpp"
#include "particle_system.hpp"
#include <cmath> // std::round
#include <random> // std::rand

int ktp::generateRand(int min, int max) {
  const auto random{rand() / RAND_MAX};
	return min + (random * (max - min));
}

float ktp::generateRand(float min, float max) {
  const auto random{static_cast<float>(rand()) / static_cast<float>(RAND_MAX)};
	return min + (random * (max - min));
}

double ktp::generateRand(double min, double max) {
  const auto random{static_cast<double>(rand()) / static_cast<double>(RAND_MAX)};
	return min + (random * (max - min));
}

/* EMITTER */

ktp::Emitter::Emitter(const std::string& type, const SDL_FPoint& pos) noexcept : position_(pos) {
  bool emitter_found{false};
  for (const auto& emitter_type: EmitterParser::emitter_types) {
    if (emitter_type.type_ == type) {
      data_ = &emitter_type;
      particles_pool_size_ = (emitter_type.max_particle_life_.value_ + 1u) * emitter_type.emission_rate_.value_;
      interval_time_ = emitter_type.emission_interval_.value_;
      emitter_found = true;
      break;
    }
  }
  if (!emitter_found) {
    logErrorMessage("Emitter type \"" + type + "\" not found! Check emitters.xml for spelling errors or missing emitters.");
    return;
  }
  inflatePool();
}

ktp::Emitter& ktp::Emitter::operator=(const Emitter& other) noexcept {
  alive_particles_count_ = other.alive_particles_count_;
  data_ = other.data_;
  first_available_ = nullptr;
  particles_pool_size_ = other.particles_pool_size_;
  position_ = other.position_;
  start_time_ = other.start_time_;
  interval_time_ = other.interval_time_;

  delete[] particles_pool_;
  particles_pool_ = new Particle[particles_pool_size_];
  // std::copy(other.particles_pool_, other.particles_pool_ + particles_pool_size_, particles_pool_); // need to benchmark this
  for (auto i = 0u; i < particles_pool_size_; ++i) {
    particles_pool_[i] = other.particles_pool_[i];
    // check if the current position is also the first available
    if (other.first_available_ == &other.particles_pool_[i]) {
      first_available_ = &particles_pool_[i];
    }
    // check the state of the union "state_"
    if (!other.particles_pool_[i].inUse()) {
      // the particle is not in use, so it must point somewhere
      auto aux_p = &other.particles_pool_[0];
      for (auto j = 0u; j < particles_pool_size_; ++j, ++aux_p) {
        if(aux_p == other.particles_pool_[i].getNext()) {
          // we found the position where it's pointing
          particles_pool_[i].setNext(&particles_pool_[j]);
          break;
        }
      }
    }
  }
  return *this;
}

ktp::Emitter& ktp::Emitter::operator=(Emitter&& other) noexcept {
  if (this != &other) {
    delete[] particles_pool_;

    alive_particles_count_ = other.alive_particles_count_;
    data_ = other.data_;
    first_available_ = other.first_available_;
    particles_pool_ = other.particles_pool_;
    particles_pool_size_ = other.particles_pool_size_;
    position_ = other.position_;
    start_time_ = other.start_time_;
    interval_time_ = other.interval_time_;

    other.particles_pool_ = nullptr;
  }
  return *this;
}

void ktp::Emitter::draw() const {
  ParticlesAtlas::particles_atlas.setBlendMode(data_->blend_mode_);
  for (auto i = 0u; i < particles_pool_size_; ++i) {
    if (particles_pool_[i].inUse()) {
      particles_pool_[i].draw();
    }
  }
}

void ktp::Emitter::generateParticles() {
  if (first_available_ == nullptr) {
    logMessage("WARNING!! NO FIRST AVAILABLE!!!!!");
    return;
  }

  const auto current_time{SDL2_Timer::getSDL2Ticks()};
  if (current_time - start_time_ > data_->life_time_) return;
  if (current_time - interval_time_ < data_->emission_interval_.value_) return;

  const auto how_many{static_cast<unsigned int>(std::round(data_->emission_rate_.value_ * generateRand(data_->emission_rate_.rand_min_, data_->emission_rate_.rand_max_)))};
  for (auto i = 0u; i < how_many; ++i) {
    ParticleData new_data{};
    new_data.start_life_ = std::round(data_->max_particle_life_.value_ * generateRand(data_->max_particle_life_.rand_min_, data_->max_particle_life_.rand_max_));
    // we need to do this in order for the particle to be update()d at least one time,
    // so we avoid the "not first available" plague
    if (new_data.start_life_ <= 0u) new_data.start_life_ = 1u;

    new_data.texture_rect_ = data_->texture_rect_;

    for (const auto& size: data_->sizes_) {
      const auto final_size {size.value_ * generateRand(size.rand_min_, size.rand_max_)};
      new_data.sizes_.push_back(final_size);
    }
    new_data.current_size_ = new_data.sizes_.front();

    new_data.colors_ = data_->colors_;
    if (data_->colors_.size() == 0) {
      // no color specified in xml
      new_data.current_color_ = ktp::Colors::white;
    } else {
      new_data.current_color_ = data_->colors_.front();
    }

    new_data.rotation_ = data_->rotation_.value_ * generateRand(data_->rotation_.rand_min_, data_->rotation_.rand_max_);

    new_data.start_rotation_speed_ = data_->start_rotation_speed_.value_ * generateRand(data_->start_rotation_speed_.rand_min_, data_->start_rotation_speed_.rand_max_);
    new_data.end_rotation_speed_ = data_->end_rotation_speed_.value_ * generateRand(data_->end_rotation_speed_.rand_min_, data_->end_rotation_speed_.rand_max_);

    const auto final_angle {generateRand(data_->angle_range_.min_, data_->angle_range_.max_)};
    const auto final_angle_cosf {SDL_cosf(final_angle)};
    const auto final_angle_sinf {SDL_sinf(final_angle)};

    for (const auto& speed: data_->speeds_) {
      const auto speed_multiplier {speed.value_ * generateRand(speed.rand_min_, speed.rand_max_)};
      const SDL_FPoint final_speed {
        (speed_multiplier * final_angle_cosf) - (speed_multiplier * final_angle_sinf),
        (speed_multiplier * final_angle_sinf) + (speed_multiplier * final_angle_cosf)
      };
      new_data.speeds_.push_back(final_speed);
    }
    new_data.current_speed_ = new_data.speeds_.front();

    new_data.position_ = position_;

    Particle* new_particle{first_available_};
    first_available_ = new_particle->getNext();
    new_particle->init(new_data);
    ++alive_particles_count_;

    if (first_available_ == nullptr) return;
  }
  interval_time_ = SDL2_Timer::getSDL2Ticks();
}

void ktp::Emitter::inflatePool() {
  particles_pool_ = new Particle[particles_pool_size_];
  
  first_available_ = &particles_pool_[0];

  for (auto i = 0u; i < particles_pool_size_; ++i) {
    particles_pool_[i].setNext(&particles_pool_[i + 1]);
  }
  particles_pool_[particles_pool_size_ - 1].setNext(nullptr);
}

void ktp::Emitter::update(float delta_time) {
  for (auto i = 0u; i < particles_pool_size_; ++i) {
    if (data_->vortex_) {
      if (particles_pool_[i].inUse() && particles_pool_[i].update(delta_time, Vortex{position_, data_->vortex_scale_, data_->vortex_speed_})) {
        particles_pool_[i].setNext(first_available_);
        first_available_ = &particles_pool_[i];
        --alive_particles_count_;
      }
    } else { // no vortex
      if (particles_pool_[i].inUse() && particles_pool_[i].update(delta_time)) {
        particles_pool_[i].setNext(first_available_);
        first_available_ = &particles_pool_[i];
        --alive_particles_count_;
      }
    }
  }
}