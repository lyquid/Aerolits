#ifndef AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_

#include "../../sdl2_wrappers/sdl2_timer.hpp"
#include "emitter_parser.hpp"
#include <box2d/box2d.h>
#include <SDL.h>
#include <memory>
#include <string>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class AeroliteGraphicsComponent;
class EmitterGraphicsComponent;
class EmitterType;
class GameEntity;
class PlayerGraphicsComponent;
class PlayerInputComponent;
class ProjectileGraphicsComponent;

class PhysicsComponent {
 public:
  virtual ~PhysicsComponent() { world_->DestroyBody(body_); }
  inline bool canBeDeleted() const { return to_be_deleted_; }
  virtual void update(const GameEntity&, float) = 0;
  virtual void setPosition(const SDL_FPoint& pos) = 0;
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

class AerolitePhysicsComponent: public PhysicsComponent {
 public:
  AerolitePhysicsComponent(AeroliteGraphicsComponent* graphics);
  static GameEntity spawnAerolite();
  inline float getMaxSpeed() const { return kMaxSpeed_; }
  virtual void setPosition(const SDL_FPoint& pos) override {}
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

class EmitterPhysicsComponent: public PhysicsComponent {
 public:
  EmitterPhysicsComponent(EmitterGraphicsComponent* graphics): graphics_(graphics) {}
  inline bool canBeDeleted() const { return lifeTimeOver() && !alive_particles_count_; }
  void generateParticles();
  inline auto getAliveParticlesCount() const { return alive_particles_count_; }
  inline SDL_FPoint getPosition() const { return position_; }
  inline bool lifeTimeOver() const { return SDL2_Timer::getSDL2Ticks() - start_time_ >= data_->life_time_; }
  static GameEntity makeEmitter(const std::string& type, const SDL_FPoint& pos);
  inline bool particlesAlive() const { return alive_particles_count_ != 0u; }
  virtual void setPosition(const SDL_FPoint& pos) override { position_ = pos; }
  void setType(const std::string& type);
  virtual void update(const GameEntity& emitter, float delta_time) override;
 private:
  void inflatePool(); // maybe static?
  inline static void setPosition(EmitterPhysicsComponent* physics, const SDL_FPoint& pos) { physics->position_ = pos; }
  static void setType(EmitterPhysicsComponent* physics, const std::string& type);
  EmitterGraphicsComponent* graphics_ {nullptr};

  unsigned int alive_particles_count_ {0};
  const EmitterType* data_ {nullptr};
  Particle* first_available_ {nullptr};
  SDL_FPoint position_ {0, 0};
  Uint32 start_time_ {SDL2_Timer::getSDL2Ticks()};
  Uint32 interval_time_ {};
};

class PlayerPhysicsComponent: public PhysicsComponent {
 public:
  PlayerPhysicsComponent(PlayerGraphicsComponent* graphics);
  virtual void setPosition(const SDL_FPoint& pos) override {}
  virtual void update(const GameEntity& player, float delta_time) override;
 private:
  friend class InputComponent;
  // inline void generateParticles() { flame_emitter_->physics_-> }
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
  // emitter stuff
  std::unique_ptr<GameEntity> flame_emitter_ {};
};

class ProjectilePhysicsComponent: public PhysicsComponent {
 public:
  ProjectilePhysicsComponent(ProjectileGraphicsComponent* graphics);
  virtual void setPosition(const SDL_FPoint& pos) override {}
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