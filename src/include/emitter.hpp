#pragma once

#include "graphics_component.hpp"
#include "object_pool.hpp"
#include "particle.hpp"
#include "physics_component.hpp"
#include "../../sdl2_wrappers/sdl2_timer.hpp"
#include <SDL.h>
#include <string>
#include <utility> // std::move

namespace ktp {

class GameEntity;
class SDL2_Renderer;

struct Vortex {
  SDL_FPoint position_ {};
  float scale_ {};
  float speed_ {};
};

struct AngleRange { float max_{0}, min_{0}; };

template <typename T>
struct RandomRangeValue { T value_{0}; float rand_min_{0}, rand_max_{1}; };
using RRVInt = RandomRangeValue<int>;
using RRVUint = RandomRangeValue<unsigned int>;
using RRVFloat = RandomRangeValue<float>;
using ColorsVector = std::vector<SDL_Color>;
using RRVFVector = std::vector<RRVFloat>;

struct EmitterType {
  // Emitter properties
  std::string   type_{};
  AngleRange    angle_range_{};
  SDL_BlendMode blend_mode_{};
  RRVUint       emission_interval_{};
  RRVUint       emission_rate_{};
  int           life_time_{};
  SDL_Rect      texture_rect_{};
  bool          vortex_{};
  float         vortex_scale_{};
  float         vortex_speed_{};

  // Particles properties
  RRVUint      max_particle_life_{};
  ColorsVector colors_{};
  RRVFVector   sizes_{};
  RRVFVector   speeds_{};
  RRVFloat     rotation_{};
  RRVFloat     start_rotation_speed_{};
  RRVFloat     end_rotation_speed_{};
};

class EmitterGraphicsComponent: public GraphicsComponent {
  friend class EmitterPhysicsComponent;
 public:
  ~EmitterGraphicsComponent() { delete[] particles_pool_; }
  virtual void update(const GameEntity& emitter, const SDL2_Renderer& renderer) override;
 private:
  SDL_BlendMode blend_mode_ {};
  Particle*     particles_pool_ {nullptr};
  unsigned int  particles_pool_size_ {0};
};

class EmitterPhysicsComponent: public PhysicsComponent {

 public:

  EmitterPhysicsComponent(const EmitterPhysicsComponent& other) = delete;
  EmitterPhysicsComponent(EmitterPhysicsComponent&& other) { *this = std::move(other); }

  EmitterPhysicsComponent& operator=(const EmitterPhysicsComponent& other) = delete;
  EmitterPhysicsComponent& operator=(EmitterPhysicsComponent&& other) noexcept;

  inline bool canBeDeleted() const { return lifeTimeOver() && !alive_particles_count_; }
  virtual void collide(GameEntity* other) override {}
  void generateParticles();
  inline auto getAliveParticlesCount() const { return alive_particles_count_; }
  inline SDL_FPoint getPosition() const { return position_; }
  inline bool lifeTimeOver() const { return SDL2_Timer::SDL2Ticks() - start_time_ >= data_->life_time_; }
  static EmitterPhysicsComponent makeEmitter(EmitterGraphicsComponent* graphics, const std::string& type, const SDL_FPoint& pos);
  inline bool particlesAlive() const { return alive_particles_count_ != 0u; }
  virtual void setPosition(const SDL_FPoint& pos) override { position_ = pos; }
  void setType(const std::string& type);
  virtual void update(const GameEntity& emitter, float delta_time) override;

 private:

  EmitterPhysicsComponent(EmitterGraphicsComponent* graphics) noexcept: graphics_(graphics) {}

  void inflatePool(); // maybe static?

  EmitterGraphicsComponent* graphics_ {nullptr};

  unsigned int       alive_particles_count_ {0};
  const EmitterType* data_ {nullptr};
  Particle*          first_available_ {nullptr};
  SDL_FPoint         position_ {0, 0};
  Uint32             start_time_ {SDL2_Timer::SDL2Ticks()};
  Uint32             interval_time_ {};
};

} // namespace ktp