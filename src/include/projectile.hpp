#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "physics_component.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include <memory>

namespace ktp {

using B2Vec2Vector = std::vector<b2Vec2>;

class EmitterPhysicsComponent;
class GameEntity;

class ProjectileGraphicsComponent: public GraphicsComponent {
  friend class ProjectilePhysicsComponent;
 public:
  ProjectileGraphicsComponent();
  virtual void update(const GameEntity& projectile) override;
 private:
  void generateOpenGLStuff(float size);
  Color color_ {ConfigParser::projectiles_config.color_};
  VAO vao_ {};
  VBO vertices_ {};
  EBO vertices_indices_ {};
  ShaderProgram shader_;
  glm::mat4 mvp_ {};
};

class ProjectilePhysicsComponent: public PhysicsComponent {

 public:

  ProjectilePhysicsComponent(GameEntity* owner, ProjectileGraphicsComponent* graphics);
  ProjectilePhysicsComponent(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent(ProjectilePhysicsComponent&& other) { *this = std::move(other); }

  ProjectilePhysicsComponent& operator=(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent& operator=(ProjectilePhysicsComponent&& other);

  inline void collide(const GameEntity* other) override { collided_ = true; }
  void detonate();
  inline auto speed() const { return speed_; }
  virtual void update(const GameEntity& projectile, float delta_time) override;

 private:

  void updateMVP();

  bool armed_ {false};
  unsigned int arm_time_ {ConfigParser::projectiles_config.arm_time_};
  bool detonated_ {false};
  // EmitterPhysicsComponent* exhaust_emitter_ {nullptr};
  ExplosionPhysicsComponent* explosion_ {nullptr};
  unsigned int fired_time_ {};
  ProjectileGraphicsComponent* graphics_ {nullptr};
  float speed_ {ConfigParser::projectiles_config.speed_};
  float sin_ {}, cos_ {};
};

} // namespace ktp
