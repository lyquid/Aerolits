#include "particle_system.hpp"
//#include <algorithm> // std::copy
#include <random>

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

ktp::Emitter::Emitter(EmitterTypes type, const SDL_FPoint& pos) noexcept : position_(pos) {
  bool emitter_found{false};
  for (const auto& emitter_type: EmitterParser::emitter_types) {
    if (emitter_type.type_ == type) {
      data_ = emitter_type;
      emitter_found = true;
      break;
    }
  }
  if (!emitter_found) {
    logErrorMessage("Emitter type not found! This should never be seen...");
  }
  inflatePool();
}

ktp::Emitter& ktp::Emitter::operator=(const Emitter& other) noexcept {
  data_ = other.data_;
  first_available_ = nullptr;
  particles_pool_size_ = other.particles_pool_size_;
  position_ = other.position_;

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

    data_ = other.data_;
    first_available_ = other.first_available_;
    particles_pool_ = other.particles_pool_;
    particles_pool_size_ = other.particles_pool_size_;
    position_ = other.position_;

    other.particles_pool_ = nullptr;
  }
  return *this;
}

void ktp::Emitter::draw() const {
  for (auto i = 0u; i < particles_pool_size_; ++i) {
    if (particles_pool_[i].inUse()) {
      particles_pool_[i].draw();
    }
  }
}

void ktp::Emitter::generateParticles() {
  if (first_available_ == nullptr) return;

  ParticleData data;
  data.start_life_ = data_.max_particle_life_.value_ * generateRand(data_.max_particle_life_.rand_min_, data_.max_particle_life_.rand_max_);
  
  data.texture_rect_ = data_.texture_rect_;
  
  data.blend_mode_ = data_.blend_mode_;

  data.start_size_ = data_.start_size_.value_ * generateRand(data_.start_size_.rand_min_, data_.start_size_.rand_max_);
  data.end_size_ = data_.end_size_.value_ * generateRand(data_.end_size_.rand_min_, data_.end_size_.rand_max_);

  data.start_color_ = data_.start_color_;
  data.end_color_ = data_.end_color_;

  data.rotation_ = data_.rotation_.value_ * generateRand(data_.rotation_.rand_min_, data_.rotation_.rand_max_);

  data.start_rotation_speed_ = data_.start_rotation_speed_.value_ * generateRand(data_.start_rotation_speed_.rand_min_, data_.start_rotation_speed_.rand_max_);
  data.end_rotation_speed_ = data_.end_rotation_speed_.value_ * generateRand(data_.end_rotation_speed_.rand_min_, data_.end_rotation_speed_.rand_max_);

  data.start_speed_.x = data_.start_speed_.value_ * generateRand(data_.start_speed_.rand_min_, data_.start_speed_.rand_max_);
  data.start_speed_.y = data_.start_speed_.value_ * generateRand(data_.start_speed_.rand_min_, data_.start_speed_.rand_max_);
  data.end_speed_.x = data_.end_speed_.value_ * generateRand(data_.end_speed_.rand_min_, data_.end_speed_.rand_max_);
  data.end_speed_.y = data_.end_speed_.value_ * generateRand(data_.end_speed_.rand_min_, data_.end_speed_.rand_max_);



  
  data.position_ = position_;
  


  

  Particle* new_particle{first_available_};
  first_available_ = new_particle->getNext();
  new_particle->init(data);
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
    if (particles_pool_[i].inUse() && particles_pool_[i].update(delta_time)) {
      particles_pool_[i].setNext(first_available_);
      first_available_ = &particles_pool_[i];
    }
  }
}