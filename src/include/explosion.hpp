#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "physics_component.hpp"
#include "resources.hpp"
#include "opengl.hpp"
#include <utility> // std::move std::exchange

namespace ktp {

class ExplosionGraphicsComponent: public GraphicsComponent {

  friend class ProjectilePhysicsComponent;
  friend class ExplosionPhysicsComponent;

 public:

  ExplosionGraphicsComponent();
  virtual void update(const GameEntity& explosion) override;

 private:

  void generateOpenGLStuff(float size);
  const Color   color_ {ConfigParser::explosion_config.color_};
  const GLuint  rays_ {ConfigParser::projectiles_config.explosion_config_.rays_};
  VAO           vao_ {};
  VBO           vertices_ {};
  GLfloatVector vertices_data_ {};
  EBO           indices_ {};
  GLuintVector  indices_data_ {};
  VBO           translations_ {};
  glm::mat4     mvp_ {};
  bool          render_ {false};
  ShaderProgram shader_ {Resources::getShader("explosion")};
  Texture2D     texture_ {Resources::getTexture("particle_02")};
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

  void updateMVP();

  bool                          detonated_ {false};
  unsigned int                  detonation_time_ {};
  ConfigParser::ExplosionConfig explosion_config_ {ConfigParser::explosion_config};
  std::vector<b2Body*>          explosion_rays_ {};
  ExplosionGraphicsComponent*   graphics_ {nullptr};
  std::vector<glm::vec3>        translations_data_ {};
};

} // namespace ktp
