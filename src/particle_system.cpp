#include "particle_system.hpp"

/* PARTICLEPOOL */

ktp::ParticlePool::ParticlePool(int pool_size): kPoolSize_(pool_size) {
  if (particles_ != nullptr) {
    delete[] particles_;
  } 
  particles_ = new Particle[kPoolSize_];
  first_available_ = &particles_[0];

  for (auto i = 0; i < kPoolSize_; ++i) {
    particles_[i].setNext(&particles_[i + 1]);
  }
  particles_[kPoolSize_ - 1].setNext(nullptr);
}

void ktp::ParticlePool::generate(const ParticleData& data) {
  if (first_available_ == nullptr) return;

  Particle* new_particle{first_available_};
  first_available_ = new_particle->getNext();
  new_particle->init(data);
}

void ktp::ParticlePool::draw(const SDL2_Renderer& renderer) const {
  for (auto i = 0; i < kPoolSize_; ++i) {
    if (particles_[i].inUse()) {
      particles_[i].draw(renderer);
    }
  }
}

void ktp::ParticlePool::update(float delta_time) {
  for (auto i = 0; i < kPoolSize_; ++i) {
    if (particles_[i].inUse() && particles_[i].update(delta_time)) {
      particles_[i].setNext(first_available_);
      first_available_ = &particles_[i];
    }
  }
}

/* EMITTER */

ktp::Emitter::Emitter(EmitterTypes type, const SDL_FPoint& pos): position_(pos) {
  bool emitter_found{false};
  for (const auto& emitter_type: EmitterParser::emitter_types) {
    if (emitter_type.type_ == type) {
      type_              = emitter_type.type_;
      angle_range_       = emitter_type.angle_range_;
      rotation_speed_    = emitter_type.rotation_speed_;
      start_speed_       = emitter_type.start_speed_;
      end_speed_         = emitter_type.end_speed_;
      start_size_        = emitter_type.start_size_;
      end_size_          = emitter_type.end_size_;
      emit_number_       = emitter_type.emit_number_;
      emit_variance_     = emitter_type.emit_variance_;
      max_particle_life_ = emitter_type.max_particle_life_;
      texture_rect_      = emitter_type.texture_rect_;
      start_color_       = emitter_type.start_color_;
      end_color_         = emitter_type.end_color_;
      blend_mode_        = emitter_type.blend_mode_;
      life_time_         = emitter_type.life_time_; // emitter life time
      emitter_found = true;
    }
  }
  if (!emitter_found) {
    logErrorMessage("Emitter type not found! This should never be seen...");
  }
}

void ktp::Emitter::generate() {
  ParticleData data;
  data.position_ = position_;
  data.texture_rect_ = texture_rect_;
  data.start_life_ = max_particle_life_.value_ * generateRand(max_particle_life_.rand_min_, max_particle_life_.rand_max_);

  data.blend_mode_ = blend_mode_;

  data.start_color_ = start_color_;
  data.end_color_ = end_color_;

  data.start_size_ = start_size_.value_ * generateRand(start_size_.rand_min_, start_size_.rand_max_);
  data.end_size_ = end_size_.value_ * generateRand(end_size_.rand_min_, end_size_.rand_max_);

  //data.start_rotation_speed_ = rotation_speed_.value_;

  particle_pool_.generate(data);
}

int ktp::Emitter::generateRand(int min, int max) {
  const auto random{rand() / RAND_MAX};
	return min + (random * (max - min));
}

float ktp::Emitter::generateRand(float min, float max) {
  const auto random{static_cast<float>(rand()) / static_cast<float>(RAND_MAX)};
	return min + (random * (max - min));
}

double ktp::Emitter::generateRand(double min, double max) {
  const auto random{static_cast<double>(rand()) / static_cast<double>(RAND_MAX)};
	return min + (random * (max - min));
}