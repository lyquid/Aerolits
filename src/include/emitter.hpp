#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "object_pool.hpp"
#include "opengl.hpp"
#include "particle.hpp"
#include "physics_component.hpp"
#include "resources.hpp"
#include "../sdl2_wrappers/sdl2_timer.hpp"
#include <SDL.h>
#include <string>
#include <utility> // std::move

namespace ktp {

class GameEntity;

struct Vortex {
  SDL_FPoint position_ {};
  float scale_ {};
  float speed_ {};
};

struct EmitterType {
  // Emitter properties
  std::string   type_ {};
  AngleRange    angle_range_ {};
  SDL_BlendMode blend_mode_ {};
  RRVUint       emission_interval_ {};
  RRVUint       emission_rate_ {};
  int           life_time_ {};
  SDL_Rect      texture_rect_ {};
  bool          vortex_ {};
  float         vortex_scale_ {};
  float         vortex_speed_ {};
  // Particles properties
  RRVUint      max_particle_life_ {};
  ColorsVector colors_ {};
  RRVFVector   sizes_ {};
  RRVFVector   speeds_ {};
  RRVFloat     rotation_ {};
  RRVFloat     start_rotation_speed_ {};
  RRVFloat     end_rotation_speed_ {};
};

class EmitterGraphicsComponent: public GraphicsComponent {

  friend class EmitterPhysicsComponent;

 public:

  EmitterGraphicsComponent();
  EmitterGraphicsComponent(const EmitterGraphicsComponent& other) = delete;
  EmitterGraphicsComponent(EmitterGraphicsComponent&& other) { *this = std::move(other); }
  ~EmitterGraphicsComponent() { delete[] particles_pool_; }

  EmitterGraphicsComponent& operator=(const EmitterGraphicsComponent& other) = delete;
  EmitterGraphicsComponent& operator=(EmitterGraphicsComponent&& other);

  virtual void update(const GameEntity& emitter) override;

 private:

  SDL_BlendMode blend_mode_ {};
  Particle*     particles_pool_ {nullptr};
  unsigned int  particles_pool_size_ {};
  unsigned int  alive_particles_count_ {};
  // opengl stuff
  VAO           vao_ {};
  VBO           vertices_ {};
  GLfloatVector vertices_data_ {};
  EBO           indices_ {};
  GLuintVector  indices_data_ { 0, 1, 2, 0, 2, 3 };
  VBO           translations_ {};
  glm::mat4     mvp_ {};
  ShaderProgram shader_ {Resources::getShader("particle")};
  Texture2D     texture_ {Resources::getTexture("particle_02")};
};

class EmitterPhysicsComponent: public PhysicsComponent {
 public:

  EmitterPhysicsComponent(const EmitterPhysicsComponent& other) = delete;
  EmitterPhysicsComponent(EmitterPhysicsComponent&& other) { *this = std::move(other); }

  EmitterPhysicsComponent& operator=(const EmitterPhysicsComponent& other) = delete;
  EmitterPhysicsComponent& operator=(EmitterPhysicsComponent&& other);

  inline bool canBeDeleted() const { return lifeTimeOver() && !alive_particles_count_; }
  virtual void collide(const GameEntity* other) override {}
  void generateParticles();
  inline auto getAliveParticlesCount() const { return alive_particles_count_; }
  inline SDL_FPoint getPosition() const { return position_; }
  inline bool lifeTimeOver() const { return SDL2_Timer::SDL2Ticks() - start_time_ >= data_->life_time_; }
  /**
   * @brief Factory function for emitters.
   * @param graphics A pointer to an EmitterGraphicsComponent.
   * @param type The type of the emitter desired.
   * @param pos Where should spawn the emitter.
   * @return A brand new EmitterPhysicsComponent to play with.
   */
  static EmitterPhysicsComponent makeEmitter(EmitterGraphicsComponent* graphics, const std::string& type, const SDL_FPoint& pos);
  inline bool particlesAlive() const { return alive_particles_count_ != 0u; }
  inline void setAngle(float angle) { angle_ = angle; }
  inline void setPosition(const SDL_FPoint& pos) { position_ = pos; }
  void setType(const std::string& type);
  void setupOpenGL();
  virtual void update(const GameEntity& emitter, float delta_time) override;

 private:

  EmitterPhysicsComponent(EmitterGraphicsComponent* graphics): graphics_(graphics) {}

  void inflatePool(); // maybe static?
  void updateMVP();

  float                     angle_ {};
  unsigned int              alive_particles_count_ {};
  const EmitterType*        data_ {nullptr};
  Particle*                 first_available_ {nullptr};
  EmitterGraphicsComponent* graphics_ {nullptr};
  Uint32                    interval_time_ {};
  SDL_FPoint                position_ {0, 0};
  Uint32                    start_time_ {SDL2_Timer::SDL2Ticks()};
  std::vector<glm::vec3>    translations_data_ {};
};

} // namespace ktp
