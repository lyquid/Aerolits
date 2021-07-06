#pragma once

#include "graphics_component.hpp"
#include "palette.hpp"
#include "physics_component.hpp"
#include <SDL.h>

namespace ktp {

using B2Vec2Vector = std::vector<b2Vec2>;

class GameEntity;
class SDL2_Renderer;

class ProjectileGraphicsComponent: public GraphicsComponent {
  friend class ProjectilePhysicsComponent;
 public:
  virtual void update(const GameEntity& projectile, const SDL2_Renderer& renderer) override;
 private:
  static constexpr SDL_Color kDefaultColor_ {Colors::copper_green};
};

class ProjectilePhysicsComponent: public PhysicsComponent {

  friend class InputComponent;

 public:

  ProjectilePhysicsComponent(ProjectileGraphicsComponent* graphics) noexcept;
  ProjectilePhysicsComponent(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent(ProjectilePhysicsComponent&& other) { *this = std::move(other); }

  ProjectilePhysicsComponent& operator=(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent& operator=(ProjectilePhysicsComponent&& other) noexcept;
  virtual void setPosition(const SDL_FPoint& pos) override {}
  virtual void update(const GameEntity& projectile, float delta_time) override;

 private:

  static void generateProjectileShape(B2Vec2Vector& shape, float size);
  void transformRenderShape();
  static constexpr float kDefaultProjectileSize_ {0.15f};
  static constexpr float kDefaultProjectileSpeed_ {30.f};

  ProjectileGraphicsComponent* graphics_ {nullptr};
};

} // namespace ktp