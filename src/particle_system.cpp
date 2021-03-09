#include "particle_system.h"

/* PARTICLE */

void ktp::Particle::draw(const SDL2_Renderer& renderer) const {
  if (frames_left_ > 150) {
    renderer.setDrawColor(ktp::Colors::white);
  } else if (frames_left_ > 100) {
    renderer.setDrawColor(ktp::Colors::yellow);
  } else if (frames_left_ > 50) {
    renderer.setDrawColor(ktp::Colors::red);
  } else {
    renderer.setDrawColor(ktp::Colors::violet);
  }
  //renderer.setDrawColor(ktp::Colors::yellow);
  renderer.drawPoint(state_.live_.position_);
}

void ktp::Particle::init(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time) {
  state_.live_.position_ = pos;
  state_.live_.delta_ = delta;
  frames_left_ = life_time;
}

/* return true if the previously live particle gave up the ghost in that frame */
bool ktp::Particle::update(float delta_time) {
  if (!inUse()) return false;

  --frames_left_;
  state_.live_.position_.x += state_.live_.delta_.x * delta_time;
  state_.live_.position_.y += state_.live_.delta_.y * delta_time;
  return frames_left_ == 0;
}

/* PARTICLEPOOL */

ktp::ParticlePool::ParticlePool(int pool_size): kPoolSize_(pool_size) {
  particles_ = new Particle[kPoolSize_];
  first_available_ = &particles_[0];

  for (auto i = 0; i < kPoolSize_; ++i) {
    particles_[i].setNext(&particles_[i + 1]);
  }
  particles_[kPoolSize_ - 1].setNext(nullptr);
}

void ktp::ParticlePool::generate(const SDL_FPoint& pos, const SDL_FPoint& delta, int life_time) {
  if (first_available_ == nullptr) return;

  Particle* new_particle = first_available_;
  first_available_ = new_particle->getNext();
  new_particle->init(pos, delta, life_time);
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
    if (particles_[i].update(delta_time)) {
      particles_[i].setNext(first_available_);
      first_available_ = &particles_[i];
    }
  }
}

/* EMITTER */

/* ktp::Emitter::Emitter(EmitterTypes type): type_(type) {

} */