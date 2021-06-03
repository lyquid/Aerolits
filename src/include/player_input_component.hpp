#ifndef AEROLITS_SRC_INCLUDE_PLAYER_INPUT_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_PLAYER_INPUT_COMPONENT_HPP_

#include "components.hpp"

namespace ktp {

class Player;

class PlayerInputComponent : public InputComponent {
 public:

  void reset();
  void update(Player& player, float delta_time);

 private:
  
  inline static constexpr float kDefaultAngularImpulse_ {5.f};
  inline static constexpr float kDefaultLinearImpulse_ {0.05f};
  inline static constexpr float kMaxDelta_ {0.1f};
  float angular_impulse_ {kDefaultAngularImpulse_};
  float linear_impulse_ {kDefaultLinearImpulse_};
  /* shooting stuff */
  inline static constexpr float kDefaultShootingInterval_ {200.f};
  float shooting_interval_ {kDefaultShootingInterval_};
  double shooting_timer_ {};
  /* lasers */
  inline static constexpr float kDefaultLaserSpeed_ {30.f};
  float laser_speed_ {kDefaultLaserSpeed_};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PLAYER_INPUT_COMPONENT_HPP_