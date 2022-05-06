#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "opengl.hpp"
#include "physics_component.hpp"
#include "resources.hpp"
#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include <cmath> // atan2f
#include <utility> // std::move std::exchange

namespace ktp {

using B2Line = Geometry::Line<b2Vec2>;

class AeroliteArrowPhysicsComponent;
class GameEntity;

class AeroliteGraphicsComponent: public GraphicsComponent {
  friend class AerolitePhysicsComponent;
 public:
  AeroliteGraphicsComponent();
  void update(const GameEntity& aerolite) override;
 private:
  const glm::vec4 color_ {Palette::colorToGlmVec4(ConfigParser::aerolites_config.colors_[1])};
  VAO vao_ {};
  VBO vertices_ {};
  VBO uv_ {};
  EBO ebo_ {};
  ShaderProgram shader_ {Resources::getShader("aerolite")};
  Texture2D texture_ {Resources::getTexture("aerolite_00")};
  std::size_t indices_count_ {};
  glm::mat4 mvp_ {};
};

class AerolitePhysicsComponent: public PhysicsComponent {

 public:

  AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics);
  AerolitePhysicsComponent(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent(AerolitePhysicsComponent&& other) { *this = std::move(other); }
  ~AerolitePhysicsComponent();

  AerolitePhysicsComponent& operator=(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent& operator=(AerolitePhysicsComponent&& other);

  static auto atan2Normalized(float y, float x) {
    const auto theta {atan2f(y, x)};
    return theta < 0.f ? theta + 2.f * b2_pi : theta;
  }
  void collide(const GameEntity* other) override { collided_ = true; }
  void reshape(float size);
  static GameEntity* spawnAerolite(const b2Vec2& where);
  static GameEntity* spawnMovingAerolite();
  void update(const GameEntity& aerolite, float delta_time) override;
  auto worldManifold() { return &world_manifold_; }
  static GLfloatVector convertToUV(const GLfloatVector& v);

 private:

  static void createB2Body(AerolitePhysicsComponent& aerolite, const GLfloatVector& triangulated_shape);
  static Geometry::Polygon generateAeroliteShape(float size, SDL_FPoint offset = {0.f, 0.f});
  static Geometry::Polygon generateAeroliteShape(float size, unsigned int sides, SDL_FPoint offset = {0.f, 0.f});
  void split();
  void updateArrow();
  void updateMVP();

  static constexpr float kMinSize_ {1.4f};
  static constexpr unsigned int kMaxSides_ {40u};
  static constexpr unsigned int kMinSides_ {30u};
  static constexpr unsigned int kScore_ {1000u};
  static constexpr unsigned int kNewBornTime_ {10000u}; // 10 seconds

  AeroliteArrowPhysicsComponent* arrow_ {nullptr};

  AeroliteGraphicsComponent* graphics_;
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
   * @brief Flag that indictes if an aerolite has entered the screen.
   * Usefull to know when an aerolite must be removed for leaving the screen.
   */
  bool new_born_ {true};
  /**
   * @brief Used for contacts information.
   */
  b2WorldManifold world_manifold_ {};
};

// ARROW

enum class Direction { Top, Left, Bottom, Right };

class AeroliteArrowGraphicsComponent: public GraphicsComponent {
  friend class AeroliteArrowPhysicsComponent;
 public:
  AeroliteArrowGraphicsComponent();
  void update(const GameEntity& aerolite_arrow) override;
  static constexpr auto kSize_ {30.f};
 private:
  Color color_ {Palette::red};
  VAO vao_ {};
  VBO vertices_ {};
  ShaderProgram shader_ {Resources::getShader("aerolite_arrow")};
  glm::mat4 mvp_ {};
};

class AeroliteArrowPhysicsComponent: public PhysicsComponent {
  friend class AerolitePhysicsComponent;
 public:
  AeroliteArrowPhysicsComponent(GameEntity* owner, AeroliteArrowGraphicsComponent* graphics);
  void collide(const GameEntity* other) override {}
  // void setPosition(const glm::vec3& pos) { position_ = pos; }
  void update(const GameEntity& aerolite_arrow, float delta_time) override;
 private:
  void updateMVP();
  float angle_ {};
  AeroliteArrowGraphicsComponent* graphics_;
  glm::vec3 position_ {};

  Direction incoming_direction_ {};
};

} // namespace ktp
