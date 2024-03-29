#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "physics_component.hpp"
#include "resources.hpp"
#include <utility>

namespace ktp {

class EmitterPhysicsComponent;

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
  ShaderProgram shader_ {Resources::getShader("projectile")};
  glm::mat4 mvp_ {};
};

class ProjectilePhysicsComponent: public PhysicsComponent {

 public:

  ProjectilePhysicsComponent(GameEntity* owner, ProjectileGraphicsComponent* graphics);
  ProjectilePhysicsComponent(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent(ProjectilePhysicsComponent&& other) { *this = std::move(other); }
  ~ProjectilePhysicsComponent() { if (body_) world_->DestroyBody(body_); }

  ProjectilePhysicsComponent& operator=(const ProjectilePhysicsComponent& other) = delete;
  ProjectilePhysicsComponent& operator=(ProjectilePhysicsComponent&& other);

  auto body() { return body_; }
  void collide(const GameEntity* other) override { collided_ = true; }
  void detonate();
  auto speed() const { return speed_; }
  virtual void update(const GameEntity& projectile, float delta_time) override;

 private:

  inline bool isOutOfScreen(float threshold = 0.f);
  void setBox2D();
  void updateMVP();

  bool armed_ {false};
  unsigned int arm_time_ {ConfigParser::projectiles_config.arm_time_};
  b2Body* body_ {nullptr};
  bool detonated_ {false};
  EmitterPhysicsComponent* exhaust_emitter_ {nullptr};
  ExplosionPhysicsComponent* explosion_ {nullptr};
  unsigned int fired_time_ {};
  ProjectileGraphicsComponent* graphics_ {nullptr};
  float speed_ {ConfigParser::projectiles_config.speed_};
  float sin_ {}, cos_ {};
};

} // namespace ktp
