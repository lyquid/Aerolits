#ifndef AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_

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
};

class PlayerInputComponent : public InputComponent {
 public:

  virtual void reset();
  virtual void update(Player& player, float delta_time);

 private:
  
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

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_