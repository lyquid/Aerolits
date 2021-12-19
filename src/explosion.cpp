#include "include/explosion.hpp"
#include "include/game.hpp"

// GRAPHICS

void ktp::XParticleGraphicsComponent::update(const GameEntity& xparticle) {
  // ParticlesAtlas::particles_atlas.setBlendMode(SDL_BlendMode::SDL_BLENDMODE_ADD);
  // ParticlesAtlas::particles_atlas.setColorMod(Colors::yellow);
  // ParticlesAtlas::particles_atlas.setAlphaMod(255);
  // ParticlesAtlas::particles_atlas.render(ren, texture_rect_, {(int)position_.x, (int)position_.y, 30, 30}, 0);
}

// PHYSICS

ktp::XParticlePhysicsComponent::XParticlePhysicsComponent(GameEntity* owner, XParticleGraphicsComponent* graphics) noexcept:
 graphics_(graphics) {
  owner_ = owner;
}

ktp::XParticlePhysicsComponent& ktp::XParticlePhysicsComponent::operator=(XParticlePhysicsComponent&& other) noexcept {
  if (this != &other) {
    // inherited members
    body_     = other.body_;
    collided_ = other.collided_;
    delta_    = std::move(other.delta_);
    owner_    = std::exchange(other.owner_, nullptr);
    size_     = other.size_;
    // own members
    detonation_time_ = other.detonation_time_;
    duration_        = other.duration_;
    graphics_        = std::exchange(other.graphics_, nullptr);
    radius_          = other.radius_;
  }
  return *this;
}

void ktp::XParticlePhysicsComponent::update(const GameEntity& xparticle, float delta_time) {
  if (Game::gameplay_timer_.milliseconds() - duration_ > detonation_time_) {
    owner_->deactivate();
  } else {
    const auto magic_num {radius_ * 1.5f};
    //graphics_->position_.x = body_->GetPosition().x * kMetersToPixels - magic_num;
    //graphics_->position_.y = body_->GetPosition().y * kMetersToPixels - magic_num;
  }
}
