#include "include/game_object.hpp"
#include "include/player.hpp"
#include "include/input_component.hpp"
#include "../sdl2_wrappers/sdl2_timer.hpp"
#include <SDL.h>

/* DEMO INPUT */



/* PLAYER INPUT */

void ktp::PlayerInputComponent::reset() {
  angular_impulse_ = kDefaultAngularImpulse_;
  linear_impulse_ = kDefaultLinearImpulse_;
  /* shooting */
  shooting_interval_ = kDefaultShootingInterval_;
  shooting_timer_ = 0.f;
  /* lasers */ 
  laser_speed_ = kDefaultLaserSpeed_;
}

void ktp::PlayerInputComponent::update(Player& player, float delta_time) {
  const auto state {SDL_GetKeyboardState(nullptr)};

  if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]){
    // thrust
    
    player.delta_.x +=  SDL_sinf(player.body_->GetAngle()) * linear_impulse_ * delta_time;
    if (player.delta_.x < -kMaxDelta_ ) {
      player.delta_.x = -kMaxDelta_;
    } else if (player.delta_.x > kMaxDelta_) {
      player.delta_.x = kMaxDelta_;
    }
    player.delta_.y += -SDL_cosf(player.body_->GetAngle()) * linear_impulse_ * delta_time;
    if (player.delta_.y < -kMaxDelta_) {
      player.delta_.y = -kMaxDelta_;
    } else if (player.delta_.y > kMaxDelta_) {
      player.delta_.y = kMaxDelta_;
    }

    player.body_->ApplyLinearImpulseToCenter(player.delta_, true);

    player.thrusting_ = true;
    
    if (player.flame_shape_.front().y < player.flame_max_lenght_) {
      player.flame_shape_.front().y += player.flame_growth_factor_;
      player.flame_shape_.back().y += player.flame_growth_factor_;
    }
    // player.exhaust_emitter_.generateParticles();
    // player.event_bus_.postEvent(kuge::EventTypes::PlayerThrust);
  } else {
    // stop thrusting
    player.thrusting_ = false;
    player.delta_ = {0.f, 0.f};
    player.flame_shape_.front().y = player.kDefaultFlameMinLength_;
    player.flame_shape_.back().y = player.kDefaultFlameMinLength_;
  }

  if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]){
    // steer left
    player.body_->SetAngularVelocity(-angular_impulse_ );
    player.stabilizer_time_ = SDL2_Timer::getSDL2Ticks();
    player.steering_ = true;
  }

  if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]){
    // steer right
    player.body_->SetAngularVelocity(angular_impulse_ );
    player.stabilizer_time_ = SDL2_Timer::getSDL2Ticks();
    player.steering_ = true;
  }

  if (state[SDL_SCANCODE_SPACE]){
    // shoot
    if (SDL_GetTicks() - shooting_timer_ > shooting_interval_) {
      Laser laser {};
      
      laser.body_ = player.world_->CreateBody(&player.laser_body_def_);
      laser.body_->CreateFixture(&player.laser_fixture_def_);
      laser.body_->SetTransform({player.body_->GetPosition().x, player.body_->GetPosition().y}, player.body_->GetAngle());
      laser.body_->SetLinearVelocity({laser_speed_ * SDL_sinf(player.body_->GetAngle()), -laser_speed_ * SDL_cosf(player.body_->GetAngle())});

      laser.render_shape_.resize(player.laser_shape_.size());
      
      player.lasers_.push_back(laser);

      shooting_timer_ = SDL2_Timer::getSDL2Ticks();

      player.event_bus_.postEvent(kuge::EventTypes::LaserFired);
    }
  }
}