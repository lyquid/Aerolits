#pragma once

#include "graphics_component.hpp"
#include "particle.hpp"
#include "physics_component.hpp"
#include "../../sdl2_wrappers/sdl2_timer.hpp"
#include <SDL.h>
#include <string>

namespace ktp {

class GameEntity;
class SDL2_Renderer;

struct Vortex {
  SDL_FPoint position_ {};
  float scale_ {};
  float speed_ {};
};

class EmitterGraphicsComponent: public GraphicsComponent {
  friend class EmitterPhysicsComponent;
 public:
  ~EmitterGraphicsComponent() { delete[] particles_pool_; }
  virtual void update(const GameEntity& emitter, const SDL2_Renderer& renderer) override;
 private:
  SDL_BlendMode blend_mode_ {};
  Particle* particles_pool_ {nullptr};
  unsigned int particles_pool_size_ {0};
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

} // namespace ktp