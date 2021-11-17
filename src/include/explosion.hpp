#pragma once

#include "graphics_component.hpp"
#include "physics_component.hpp"
#include <SDL.h>
#include <utility> // std::move std::exchange

namespace ktp {

class XParticleGraphicsComponent: public GraphicsComponent {
  friend class ProjectilePhysicsComponent;
  friend class XParticlePhysicsComponent;
 public:
  virtual void update(const GameEntity& xparticle, const SDL2_Renderer& renderer) override;
 private:
  SDL_FPoint position_ {};
  static constexpr SDL_Rect texture_rect_ {256, 64, 128, 128};
};

class XParticlePhysicsComponent: public PhysicsComponent {

  friend class ProjectilePhysicsComponent;

 public:

  XParticlePhysicsComponent(GameEntity* owner, XParticleGraphicsComponent* graphics) noexcept;
  XParticlePhysicsComponent(const XParticlePhysicsComponent& other) = delete;
  XParticlePhysicsComponent(XParticlePhysicsComponent&& other) { *this = std::move(other); }

  XParticlePhysicsComponent& operator=(const XParticlePhysicsComponent& other) = delete;
  XParticlePhysicsComponent& operator=(XParticlePhysicsComponent&& other) noexcept;

  virtual void collide(const GameEntity* other) override { collided_ = true; }
  inline auto detonationTime() const { return detonation_time_; }
  inline auto duration() const { return duration_; }
  inline auto radius() const { return radius_; }
  inline void setDetonationTime(unsigned int time) { detonation_time_ = time; }
  inline void setDuration(unsigned int duration) { duration_ = duration; }
  inline void setRadius(float radius) { radius_ = radius; }
  virtual void update(const GameEntity& xparticle, float delta_time) override;

 private:

  unsigned int detonation_time_ {};
  unsigned int duration_ {};
  XParticleGraphicsComponent* graphics_ {nullptr};
  float radius_ {};
};

} // namespace ktp
