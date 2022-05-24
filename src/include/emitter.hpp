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

struct EmitterType {
  // Emitter properties
  std::string   type_ {};
  AngleRange    angle_range_ {};
  SDL_BlendMode blend_mode_ {};
  RRVUint       emission_interval_ {};
  RRVUint       emission_rate_ {};
  unsigned int  life_time_ {};
  SDL_Rect      texture_rect_ {};
  bool          vortex_ {};
  float         vortex_scale_ {};
  float         vortex_speed_ {};
  // Particles properties
  RRVFloat     max_particle_life_ {};
  GLMColors    colors_ {};
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

  EmitterGraphicsComponent& operator=(const EmitterGraphicsComponent& other) = delete;
  EmitterGraphicsComponent& operator=(EmitterGraphicsComponent&& other);

  void update(const GameEntity& emitter) override;

 private:

  SDL_BlendMode blend_mode_ {};
  unsigned int* particles_pool_size_ {nullptr};
  // opengl stuff
  VAO           vao_ {};
  VBO           vertices_ {};
  GLfloatVector vertices_data_ {};
  EBO           indices_ {};
  GLuintVector  indices_data_ { 0, 1, 2, 0, 2, 3 };
  VBO           subdata_ {};
  glm::mat4     mvp_ {};
  ShaderProgram shader_ {Resources::getShader("particle")};
  Texture2D     texture_ {Resources::getTexture("particle_02")};
};

class EmitterPhysicsComponent: public PhysicsComponent {
 public:

  EmitterPhysicsComponent(GameEntity* owner, EmitterGraphicsComponent* graphics): graphics_(graphics) { owner_ = owner; }
  EmitterPhysicsComponent(const EmitterPhysicsComponent& other) = delete;
  EmitterPhysicsComponent(EmitterPhysicsComponent&& other) { *this = std::move(other); }
  ~EmitterPhysicsComponent() { delete[] particles_pool_; }

  EmitterPhysicsComponent& operator=(const EmitterPhysicsComponent& other) = delete;
  EmitterPhysicsComponent& operator=(EmitterPhysicsComponent&& other);

  void canBeDeactivated() { can_be_deactivated_ = true; }
  virtual void collide(const GameEntity* other) override {}
  void generateParticles();
  auto getPosition() const { return position_; }
  void init(const std::string& type, const glm::vec3& pos);
  void setAngle(float angle) { angle_ = angle; }
  void setPosition(const glm::vec3& pos) { position_ = pos; }
  virtual void update(const GameEntity& emitter, float delta_time) override;

 private:

  void inflatePool();
  void setType(const std::string& type);
  void setupOpenGL();

  /**
   * @brief The number of components per vertex of the subdata.
   * Ie: xyz + rgba + size = 8.
   */
  static constexpr auto     kComponents {8u};

  unsigned int              alive_particles_count_ {};
  float                     angle_ {};
  bool                      can_be_deactivated_ {false};
  const EmitterType*        data_ {nullptr};
  Particle*                 first_available_ {nullptr};
  EmitterGraphicsComponent* graphics_ {nullptr};
  Uint32                    interval_time_ {};
  Particle*                 particles_pool_ {nullptr};
  unsigned int              particles_pool_size_ {};
  glm::vec3                 position_ {0.f, 0.f, 0.f};
  Uint32                    start_time_ {SDL2_Timer::SDL2Ticks()};
  GLfloatVector             subdata_ {};
};

} // namespace ktp
