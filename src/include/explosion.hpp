#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "physics_component.hpp"
#include <utility> // std::move std::exchange

namespace ktp {

class ExplosionGraphicsComponent: public GraphicsComponent {
  friend class ProjectilePhysicsComponent;
  friend class ExplosionPhysicsComponent;
 public:
  virtual void update(const GameEntity& explosion) override;
 private:
  SDL_FPoint position_ {};
  //static constexpr SDL_Rect texture_rect_ {256, 64, 128, 128};
};

class ExplosionPhysicsComponent: public PhysicsComponent {

  friend class ProjectilePhysicsComponent;

 public:

  ExplosionPhysicsComponent(GameEntity* owner, ExplosionGraphicsComponent* graphics);
  ExplosionPhysicsComponent(const ExplosionPhysicsComponent& other) = delete;
  ExplosionPhysicsComponent(ExplosionPhysicsComponent&& other) { *this = std::move(other); }
  ~ExplosionPhysicsComponent() override { for (auto ray: explosion_rays_) if (ray) world_->DestroyBody(ray); }

  ExplosionPhysicsComponent& operator=(const ExplosionPhysicsComponent& other) = delete;
  ExplosionPhysicsComponent& operator=(ExplosionPhysicsComponent&& other);

  virtual void collide(const GameEntity* other) override { collided_ = true; }
  void detonate(Uint32 time, const b2Vec2& position);
  virtual void update(const GameEntity& explosion, float delta_time) override;

 private:

  bool detonated_ {false};
  unsigned int detonation_time_ {};
  ConfigParser::ExplosionConfig explosion_config_ {ConfigParser::explosion_config};
  std::vector<b2Body*> explosion_rays_ {};
  ExplosionGraphicsComponent* graphics_ {nullptr};
};

} // namespace ktp
