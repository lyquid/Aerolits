#pragma once

#include "config_parser.hpp"
#include "entity_manager.hpp"
#include "game.hpp"
#include "opengl.hpp"
#include "resources.hpp"
#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include <utility> // std::move std::exchange

namespace ktp {

using B2Line = Geometry::Line<b2Vec2>;

class AerolitePhysicsComponent;
class AeroliteRenderComponent;

class AeroliteManager {
 public:
  static void activate(EntityId id);
  static void deactivate(EntityId id);
  static void draw();
  static AerolitePhysicsComponent* findPhysics(EntityId id);
  static AeroliteRenderComponent* findRender(EntityId id);
  static void init();
  static void update(float delta_time);
 private:
  static std::vector<AerolitePhysicsComponent> aerolite_physics_;
  static std::vector<AeroliteRenderComponent>  aerolite_render_;
  static unsigned int count_;
};

class AeroliteRenderComponent {
  friend class AerolitePhysicsComponent;
 public:
  AeroliteRenderComponent();
  static auto& shader() { return shader_; }
  static auto& texture() { return texture_; }
  void activate(EntityId id) { id_ = id; active_ = true; }
  bool active() const { return active_; }
  void deactivate() { active_ = false; }
  auto id() const { return id_; }
  void update();
 private:
  static ShaderProgram shader_;
  static Texture2D texture_;
  EntityId id_ {};
  bool active_ {false};
  const glm::vec4 color_ {Palette::colorToGlmVec4(ConfigParser::aerolites_config.colors_[1])};
  VAO vao_ {};
  VBO vertices_ {};
  VBO uv_ {};
  EBO ebo_ {};
  GLuint indices_count_ {};
  glm::mat4 mvp_ {};
};

class AerolitePhysicsComponent {

 public:

  AerolitePhysicsComponent();
  AerolitePhysicsComponent(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent(AerolitePhysicsComponent&& other) { *this = std::move(other); }
  ~AerolitePhysicsComponent() { if (body_) Game::world()->DestroyBody(body_); }

  AerolitePhysicsComponent& operator=(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent& operator=(AerolitePhysicsComponent&& other);

  void activate(EntityId id) { id_ = id; active_ = true; }
  bool active() const { return active_; }
  void deactivate() { active_ = false; }
  void collide(const GameEntity* other) { collided_ = true; }
  auto id() const { return id_; }
  void reshape(float size);
  static GameEntity* spawnAerolite(const b2Vec2& where);
  static GameEntity* spawnMovingAerolite();
  void update(float delta_time);
  auto worldManifold() { return &world_manifold_; }
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

  /**
   * @brief
   */
  EntityId id_ {};

  /**
   * @brief
   */
  bool active_ {false};

  /**
   * @brief This is used to know when an aerolite has left the screen.
   */
  b2AABB aabb_ {};

  /**
   * @brief The b2Body of the aerolite.
   */
  b2Body* body_ {nullptr};

  /**
   * @brief When the aerolite was born.
   */
  Uint32 born_time_ {};

  /**
   * @brief Has the aerolite collided?
   */
  bool collided_ {false};

  /**
   * @brief Flag that indictes if an aerolite has entered the screen.
   * Usefull to know when an aerolite must be removed for leaving the screen.
   */
  bool new_born_ {true};

  /**
   * @brief
   */
  float size_ {};

  /**
   * @brief Used for contacts information.
   */
  b2WorldManifold world_manifold_ {};
};

} // namespace ktp
