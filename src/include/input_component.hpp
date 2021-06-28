#ifndef AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_

#include "../../sdl2_wrappers/sdl2_timer.hpp"

namespace ktp {

class GameEntity;
class Player;
class PlayerPhysicsComponent;

enum class InputComponents {
  Demo,
  Player,
  count
};

class InputComponent {
 public:
  InputComponent(PlayerPhysicsComponent* physics): physics_(physics) {}
  virtual ~InputComponent() {}
  virtual void update(GameEntity&, float) = 0;

 protected:
  void shoot(GameEntity& player);
  void steer(float angular_impulse);
  void stopSteering(float delta_time);
  void stopThrusting(GameEntity& player);
  void thrust(GameEntity& player, float delta_time);

  PlayerPhysicsComponent* physics_ {nullptr};

  static constexpr float kDefaultAngularImpulse_ {5.f};
  static constexpr float kDefaultLinearImpulse_ {0.05f};
  static constexpr float kMaxDelta_ {0.1f};
  float  angular_impulse_ {kDefaultAngularImpulse_};
  float  linear_impulse_ {kDefaultLinearImpulse_};
  Uint32 stabilizer_time_ {};
  bool   steering_ {true};
  /* shooting stuff */
  static constexpr float kDefaultShootingInterval_ {200.f};
  float shooting_interval_ {kDefaultShootingInterval_};
  double shooting_timer_ {};
};

class DemoInputComponent: public InputComponent {
 public:
  DemoInputComponent(PlayerPhysicsComponent* physics): InputComponent(physics) {}
  virtual void update(GameEntity& player, float delta_time) override;
 private:
  static constexpr int kThrustingInterval_ {5000};
  bool thrust_ {};
  Uint32 thrusting_timer_ {SDL2_Timer::getSDL2Ticks()};
};

class PlayerInputComponent: public InputComponent {
 public:
  PlayerInputComponent(PlayerPhysicsComponent* physics): InputComponent(physics) {}
  virtual void update(GameEntity& player, float delta_time) override;
 private:
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_