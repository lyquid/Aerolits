#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "opengl.hpp"
#include "physics_component.hpp"
#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include <utility> // std::move std::exchange

namespace ktp {

using B2Vec2Vector = std::vector<b2Vec2>;
using B2Line = Geometry::Line<b2Vec2>;

class GameEntity;

class AeroliteGraphicsComponent: public GraphicsComponent {
  friend class AerolitePhysicsComponent;
 public:
  AeroliteGraphicsComponent() noexcept;
  virtual void update(const GameEntity& aerolite) override;
 private:
  const b2Color color_ {SDL2ColorToB2Color(ConfigParser::aerolites_config.colors_[1])};
  VAO vao_ {};
  VBO vertices_ {};
  VBO uv_ {};
  EBO ebo_ {};
  ShaderProgram shader_;
  Texture2D texture_;
  GLuint indices_count_ {};
  glm::mat4 mvp_ {};
};

class AerolitePhysicsComponent: public PhysicsComponent {

 public:

  AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics) noexcept;
  AerolitePhysicsComponent(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent(AerolitePhysicsComponent&& other) { *this = std::move(other); }

  AerolitePhysicsComponent& operator=(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent& operator=(AerolitePhysicsComponent&& other) noexcept;

  inline void collide(const GameEntity* other) override { collided_ = true; }
  void reshape(float size);
  static GameEntity* spawnAerolite(const b2Vec2& where);
  static GameEntity* spawnMovingAerolite();
  virtual void update(const GameEntity& aerolite, float delta_time) override;
  inline auto worldManifold() { return &world_manifold_; }
static GLfloatVector convertToUV(const GLfloatVector& v);
 private:


  static void createB2Body(AerolitePhysicsComponent& aerolite, const GLfloatVector& triangulated_shape);
  static Geometry::Polygon generateAeroliteShape(float size, SDL_FPoint offset = {0.f, 0.f});
  static Geometry::Polygon generateAeroliteShape(float size, unsigned int sides, SDL_FPoint offset = {0.f, 0.f});
  void split();
  void updateMVP();

  static constexpr float kMinSize_ {1.4f};
  static constexpr unsigned int kMaxSides_ {40u};
  static constexpr unsigned int kMinSides_ {30u};
  static constexpr unsigned int kScore_ {1000u};
  static constexpr unsigned int kNewBornTime_ {10000u}; // 10 seconds

  AeroliteGraphicsComponent* graphics_ {nullptr};
  /**
   * @brief This is used to know when an aerolite has left the screen.
   */
  b2AABB aabb_ {};
  /**
   * @brief When the aerolite was born.
   */
  Uint32 born_time_ {};
  /**
   * @brief Flag that indictes if an aerolite has entered the screen.
   * Usefull to know when an aerolite must be removed for leaving the screen.
   */
  bool new_born_ {true};
  /**
   * @brief Used for contacts information.
   */
  b2WorldManifold world_manifold_ {};
};

} // namespace ktp
