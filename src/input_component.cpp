#include "include/game_entity.hpp"
#include "include/input_component.hpp"
#include <SDL.h>

/* INPUT COMPONENT */

/* void ktp::InputComponent::shoot(GameEntity& player) {
  if (SDL_GetTicks() - shooting_timer_ > shooting_interval_) {
    Laser laser {};
    // b2 body
    laser.body_ = player.world_->CreateBody(&player.laser_body_def_);
    laser.body_->CreateFixture(&player.laser_fixture_def_);
    laser.body_->SetTransform({player.body_->GetPosition().x, player.body_->GetPosition().y}, player.body_->GetAngle());
    laser.body_->SetLinearVelocity({laser_speed_ * SDL_sinf(player.body_->GetAngle()), -laser_speed_ * SDL_cosf(player.body_->GetAngle())});

    laser.render_shape_.resize(player.laser_shape_.size());
    
    player.lasers_.push_back(laser);

    shooting_timer_ = SDL2_Timer::getSDL2Ticks();

    // player.event_bus_.postEvent(kuge::EventTypes::LaserFired);
  }
} */

void ktp::InputComponent::steer(GameEntity& player, float angular_impulse) {
  physics_->body_->SetAngularVelocity(angular_impulse);
  //player.body_->SetAngularVelocity(angular_impulse);
  stabilizer_time_ = SDL2_Timer::getSDL2Ticks();
  steering_ = true;
}

void ktp::InputComponent::stopSteering(GameEntity& player, float delta_time) {
  if (steering_ && SDL2_Timer::getSDL2Ticks() - stabilizer_time_ > delta_time) {
    //player.body_->SetAngularVelocity(0.f);
    physics_->body_->SetAngularVelocity(0.f);
    steering_ = false;
  }
}

void ktp::InputComponent::stopThrusting(GameEntity& player) {
  player.delta_ = {0.f, 0.f};
  //player.flame_shape_.front().y = player.kDefaultFlameMinLength_;
  //player.flame_shape_.back().y = player.kDefaultFlameMinLength_;
  //player.thrusting_ = false;
}

void ktp::InputComponent::thrust(GameEntity& player, float delta_time) {
  //player.delta_.x += SDL_sinf(player.body_->GetAngle()) * linear_impulse_ * delta_time;
  player.delta_.x += SDL_sinf(physics_->body_->GetAngle()) * linear_impulse_ * delta_time;
  if (player.delta_.x < -kMaxDelta_ ) {
    player.delta_.x = -kMaxDelta_;
  } else if (player.delta_.x > kMaxDelta_) {
    player.delta_.x = kMaxDelta_;
  }
  player.delta_.y += -SDL_cosf(physics_->body_->GetAngle()) * linear_impulse_ * delta_time;
  if (player.delta_.y < -kMaxDelta_) {
    player.delta_.y = -kMaxDelta_;
  } else if (player.delta_.y > kMaxDelta_) {
    player.delta_.y = kMaxDelta_;
  }

  physics_->body_->ApplyLinearImpulseToCenter({player.delta_.x, player.delta_.y}, true);

  // player.thrusting_ = true;
  
  /* if (player.flame_shape_.front().y < player.flame_max_lenght_) {
    player.flame_shape_.front().y += player.flame_growth_factor_;
    player.flame_shape_.back().y += player.flame_growth_factor_;
  }
  player.exhaust_emitter_.generateParticles(); */
  // player.event_bus_.postEvent(kuge::EventTypes::PlayerThrust);
}

/* DEMO INPUT */

void ktp::DemoInputComponent::update(GameEntity& player, float delta_time) {
  stopSteering(player, delta_time);
  // shoot(player);

  thrust_ ? thrust(player, delta_time) : stopThrusting(player);

  if (SDL2_Timer::getSDL2Ticks() - thrusting_timer_ > kThrustingInterval_) {
    thrust_ = !thrust_; 
    thrusting_timer_ = SDL2_Timer::getSDL2Ticks();
  }
}

/* PLAYER INPUT */

void ktp::PlayerInputComponent::update(GameEntity& player, float delta_time) {

  stopSteering(player, delta_time);

  const auto state {SDL_GetKeyboardState(nullptr)};

  if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
    thrust(player, delta_time);
  } else {
    stopThrusting(player);
  }

  if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
    // steer left
    steer(player, -angular_impulse_);
  }

  if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
    // steer right
    steer(player, angular_impulse_);
  }

  if (state[SDL_SCANCODE_SPACE]) {
    // shoot(player);
  }
}