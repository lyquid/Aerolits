#ifndef AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_

namespace ktp {

class GameEntity;
class PlayerPhysicsComponent;

class InputComponent {

 public:

  InputComponent(PlayerPhysicsComponent* physics): physics_(physics) {}
  virtual ~InputComponent() {}
  InputComponent(const InputComponent& other) = delete;
  InputComponent(InputComponent&& other) = delete;

  InputComponent& operator=(const InputComponent& other) = delete;
  InputComponent& operator=(InputComponent&& other) = delete;

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

  // shooting stuff
  static constexpr float kDefaultShootingInterval_ {200.f};
  float  shooting_interval_ {kDefaultShootingInterval_};
  double shooting_timer_ {};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_INPUT_COMPONENT_HPP_