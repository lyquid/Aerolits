#ifndef AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_

#include "../../sdl2_wrappers/sdl2_timer.hpp"

namespace ktp {

class GameObject;
class Player;

enum class InputComponents {
  Demo,
  Player,
  count
};

class InputComponent {
 public:
  virtual ~InputComponent() {}
  virtual void update(Player&, float) = 0;

 protected:
  void shoot(Player& player);
  void steer(Player& player, float angular_impulse);
  void stopThrusting(Player& player);
  void thrust(Player& player, float delta_time);

  static constexpr float kDefaultAngularImpulse_ {5.f};
  static constexpr float kDefaultLinearImpulse_ {0.05f};
  static constexpr float kMaxDelta_ {0.1f};
  float angular_impulse_ {kDefaultAngularImpulse_};
  float linear_impulse_ {kDefaultLinearImpulse_};
  /* shooting stuff */
  static constexpr float kDefaultShootingInterval_ {200.f};
  float shooting_interval_ {kDefaultShootingInterval_};
  double shooting_timer_ {};
  /* lasers */
  static constexpr float kDefaultLaserSpeed_ {30.f};
  float laser_speed_ {kDefaultLaserSpeed_};
};

class DemoInputComponent : public InputComponent {
 public:
  virtual void update(Player& player, float delta_time);
 private:
  static constexpr int kThrustingInterval_ {5000};
  bool thrust_ {};
  Uint32 thrusting_timer_ {SDL2_Timer::getSDL2Ticks()};
};

class PlayerInputComponent : public InputComponent {
 public:
  virtual void update(Player& player, float delta_time);
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_