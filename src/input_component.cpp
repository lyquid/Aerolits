#include "include/game.hpp"
#include "include/game_entity.hpp"
#include "include/input_component.hpp"
#include <SDL.h>
#include <utility> // std::move

/* INPUT COMPONENT */

void ktp::InputComponent::shoot(GameEntity& player) {
  if (SDL_GetTicks() - shooting_timer_ > shooting_interval_) {

    const auto projectile {GameEntity::createEntity(EntityTypes::Projectile)};
    if (!projectile) return;

    const auto sin {SDL_sinf(physics_->body_->GetAngle())};
    const auto cos {SDL_cosf(physics_->body_->GetAngle())};

    projectile->physics()->body()->SetTransform({
      physics_->body_->GetPosition().x + projectile->physics()->size() * 5.2f * sin,
      physics_->body_->GetPosition().y - projectile->physics()->size() * 5.2f * cos},
      physics_->body_->GetAngle()
    );

    projectile->physics()->body()->SetLinearVelocity({
       ConfigParser::projectiles_config.speed_ * sin,
      -ConfigParser::projectiles_config.speed_ * cos
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
  physics_->flame_shape_.front().y = physics_->flame_min_lenght_;
  physics_->flame_shape_.back().y = physics_->flame_min_lenght_;
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

  if (physics_->flame_shape_.front().y < physics_->flame_max_lenght_) {
    physics_->flame_shape_.front().y += physics_->flame_growth_factor_;
    physics_->flame_shape_.back().y += physics_->flame_growth_factor_;
  }
}
