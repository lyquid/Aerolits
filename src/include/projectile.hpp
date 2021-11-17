#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "palette.hpp"
#include "physics_component.hpp"
#include <SDL.h>
#include <memory>
#include <vector>

namespace ktp {

using B2Vec2Vector = std::vector<b2Vec2>;

class EmitterGraphicsComponent;
class EmitterPhysicsComponent;
class GameEntity;
class SDL2_Renderer;

class ProjectileGraphicsComponent: public GraphicsComponent {
  friend class ProjectilePhysicsComponent;
 public:
  ProjectileGraphicsComponent() noexcept;
  virtual void update(const GameEntity& projectile, const SDL2_Renderer& renderer) override;
 private:
  SDL_Color color_ {ConfigParser::projectiles_config.color_};
  std::unique_ptr<EmitterGraphicsComponent> exhaust_emitter_ {nullptr};
};

class ProjectilePhysicsComponent: public PhysicsComponent {

 public:

  ProjectilePhysicsComponent(GameEntity* owner, ProjectileGraphicsComponent* graphics) noexcept;
  ProjectilePhysicsComponent(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent(ProjectilePhysicsComponent&& other) { *this = std::move(other); }

  ProjectilePhysicsComponent& operator=(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent& operator=(ProjectilePhysicsComponent&& other) noexcept;

  inline void collide(const GameEntity* other) override { collided_ = true; }
  void detonate();
  inline auto speed() const { return speed_; }
  virtual void update(const GameEntity& projectile, float delta_time) override;

 private:

  static void generateProjectileShape(B2Vec2Vector& shape, float size);
  void transformRenderShape();

  bool armed_ {false};
  unsigned int arm_time_ {ConfigParser::projectiles_config.arm_time_};
  bool detonated_ {false};
  std::unique_ptr<EmitterPhysicsComponent> exhaust_emitter_ {nullptr};
  ConfigParser::ExplosionConfig explosion_config_ {};
  unsigned int fired_time_ {};
  ProjectileGraphicsComponent* graphics_ {nullptr};
  float speed_ {ConfigParser::projectiles_config.speed_};
  float sin_ {}, cos_ {};
};

} // namespace ktp
