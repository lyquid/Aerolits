#ifndef AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_

#include <box2d/box2d.h>
#include <SDL.h>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class GameEntity;
class PlayerGraphicsComponent;
class PlayerInputComponent;

class PhysicsComponent {
 public:
  virtual ~PhysicsComponent() {}
  virtual void update(const GameEntity&, float) = 0;
  static void setScreenSize(const SDL_FPoint& screen_size);
  static void setWorld(b2World* world) { world_ = world; }
 protected:
  friend class InputComponent;
  static SDL_FPoint b2_screen_size_;
  static b2World* world_;
  b2Body* body_ {nullptr};
};

class PlayerPhysicsComponent : public PhysicsComponent {
 public:
  PlayerPhysicsComponent(PlayerGraphicsComponent* graphics);
  virtual void update(const GameEntity& player, float delta_time) override;
 private:
  friend class InputComponent;
  static void generatePlayerShape(FPointsVector& shape, FPointsVector& flame_shape, float size);
  void checkWrap();
  void setBox2D();
  void transformRenderShape();

  static constexpr float kDefaultPlayerSize_ {1.2f};
  PlayerGraphicsComponent* graphics_ {nullptr};
  FPointsVector shape_ {};
  FPointsVector flame_shape_ {};
  float size_ {kDefaultPlayerSize_};
  bool thrusting_ {false};
  // flame stuff
  static constexpr float kDefaultFlameGrowthFactor_ {0.02f};
  static constexpr float kDefaultFlameMaxLength_ {kDefaultPlayerSize_};
  static constexpr float kDefaultFlameMinLength_ {kDefaultPlayerSize_ * 0.4f};
  float flame_growth_factor_ {kDefaultFlameGrowthFactor_};
  float flame_max_lenght_ {kDefaultFlameMaxLength_};
};

/* class AerolitePhysicsComponent : public PhysicsComponent {
 public:
  virtual void update(const GameEntity& aerolite, float delta_time) override;
}; */

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_