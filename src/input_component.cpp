#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/input_component.hpp"
#include <SDL.h>
#include <utility> // std::move

/* INPUT COMPONENT */

void ktp::InputComponent::shoot(GameEntity& player) {
  if (SDL_GetTicks() - shooting_timer_ > shooting_interval_) {

    const auto projectile_phy {static_cast<ProjectilePhysicsComponent*>(GameEntity::createEntity(EntityTypes::Projectile)->physics())};
    if (!projectile_phy) return;

    const auto sin {SDL_sinf(physics_->body_->GetAngle())};
    const auto cos {SDL_cosf(physics_->body_->GetAngle())};

    projectile_phy->body()->SetTransform({
      physics_->body_->GetPosition().x + projectile_phy->size() * 5.2f * sin,
      physics_->body_->GetPosition().y - projectile_phy->size() * 5.2f * cos},
      physics_->body_->GetAngle()
    );

    projectile_phy->body()->SetLinearVelocity({
       projectile_phy->speed() * sin * 30,
      -projectile_phy->speed() * cos * 30
    });

    shooting_timer_ = SDL2_Timer::SDL2Ticks();
    // player.event_bus_.postEvent(kuge::EventTypes::LaserFired);
  }
}

void ktp::InputComponent::steer(float angular_impulse) {
  physics_->body_->SetAngularVelocity(angular_impulse);
  stabilizer_time_ = SDL2_Timer::SDL2Ticks();
  steering_ = true;
}

void ktp::InputComponent::stopSteering(float delta_time) {
  if (steering_ && SDL2_Timer::SDL2Ticks() - stabilizer_time_ > delta_time) {
    physics_->body_->SetAngularVelocity(0.f);
    steering_ = false;
  }
}

void ktp::InputComponent::stopThrusting(GameEntity& player) {
  physics_->delta_ = {0.f, 0.f};
  physics_->thrusting_ = false;
}

void ktp::InputComponent::thrust(GameEntity& player, float delta_time) {
  physics_->delta_.x += SDL_sinf(physics_->body_->GetAngle()) * linear_impulse_ * delta_time;
  if (physics_->delta_.x < -kMaxDelta_ ) {
    physics_->delta_.x = -kMaxDelta_;
  } else if (physics_->delta_.x > kMaxDelta_) {
    physics_->delta_.x = kMaxDelta_;
  }
  physics_->delta_.y += -SDL_cosf(physics_->body_->GetAngle()) * linear_impulse_ * delta_time;
  if (physics_->delta_.y < -kMaxDelta_) {
    physics_->delta_.y = -kMaxDelta_;
  } else if (physics_->delta_.y > kMaxDelta_) {
    physics_->delta_.y = kMaxDelta_;
  }

  physics_->body_->ApplyLinearImpulseToCenter({physics_->delta_.x, physics_->delta_.y}, true);

  physics_->thrusting_ = true;
}
