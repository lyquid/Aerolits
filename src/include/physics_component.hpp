#ifndef AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_

#include <box2d/box2d.h>
#include <SDL.h>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class GameEntity;
class AeroliteGraphicsComponent;
class PlayerGraphicsComponent;
class PlayerInputComponent;
class ProjectileGraphicsComponent;

class PhysicsComponent {
 public:
  virtual ~PhysicsComponent() { world_->DestroyBody(body_); }
  inline bool canBeDeleted() const { return to_be_deleted_; }
  virtual void update(const GameEntity&, float) = 0;
  static void setScreenSize(const SDL_FPoint& screen_size);
  static void setWorld(b2World* world) { world_ = world; }
  inline b2Body* getBody() const { return body_; }
 protected:
  friend class InputComponent;
  static SDL_FPoint b2_screen_size_;
  static b2World* world_;
  b2Body* body_ {nullptr};
  FPointsVector shape_ {};
  float size_ {};
  bool to_be_deleted_ {false};
};

class PlayerPhysicsComponent: public PhysicsComponent {
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
  FPointsVector flame_shape_ {};
  bool thrusting_ {false};
  // flame stuff
  static constexpr float kDefaultFlameGrowthFactor_ {0.02f};
  static constexpr float kDefaultFlameMaxLength_ {kDefaultPlayerSize_};
  static constexpr float kDefaultFlameMinLength_ {kDefaultPlayerSize_ * 0.4f};
  float flame_growth_factor_ {kDefaultFlameGrowthFactor_};
  float flame_max_lenght_ {kDefaultFlameMaxLength_};
};

class AerolitePhysicsComponent: public PhysicsComponent {
 public:
  AerolitePhysicsComponent(AeroliteGraphicsComponent* graphics);
  static GameEntity spawnAerolite();
  inline float getMaxSpeed() const { return kMaxSpeed_; }
  virtual void update(const GameEntity& aerolite, float delta_time) override;
 private:
  static void generateAeroliteShape(FPointsVector& shape, float size);
  void transformRenderShape();
  static constexpr float kKgPerMeter2_ {20.f};
  static constexpr float kMaxRotationSpeed_ {2.f};
  static constexpr float kMaxSize_ {5.f};
  static constexpr float kMaxSpeed_ {10.f};
  AeroliteGraphicsComponent* graphics_ {nullptr};
  b2AABB aabb_ {};
};

class ProjectilePhysicsComponent: public PhysicsComponent {
 public:
  ProjectilePhysicsComponent(ProjectileGraphicsComponent* graphics);
  virtual void update(const GameEntity& projectile, float delta_time) override;
 private:
  friend class InputComponent;
  static void generateProjectileShape(FPointsVector& shape, float size);
  void transformRenderShape();
  static constexpr float kDefaultProjectileSize_ {0.15f};
  static constexpr float kDefaultProjectileSpeed_ {30.f};
  ProjectileGraphicsComponent* graphics_ {nullptr};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_