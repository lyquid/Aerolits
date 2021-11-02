#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "palette.hpp"
#include "physics_component.hpp"
#include <box2d/box2d.h>
#include <SDL.h>
#include <utility> // std::move std::exchange
#include <vector>

namespace ktp {

using B2Vec2Vector = std::vector<b2Vec2>;

class GameEntity;
class SDL2_Renderer;

class AeroliteGraphicsComponent: public GraphicsComponent {
 public:
  virtual void update(const GameEntity& aerolite, const SDL2_Renderer& renderer) override;
 private:
  SDL_Color color_ {ConfigParser::aerolites_config.colors_.front()};
};

class AerolitePhysicsComponent: public PhysicsComponent {

 public:

  AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics) noexcept;
  AerolitePhysicsComponent(GameEntity* owner, AeroliteGraphicsComponent* graphics, float size) noexcept;
  AerolitePhysicsComponent(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent(AerolitePhysicsComponent&& other) { *this = std::move(other); }

  AerolitePhysicsComponent& operator=(const AerolitePhysicsComponent& other) = delete;
  AerolitePhysicsComponent& operator=(AerolitePhysicsComponent&& other) noexcept;

  inline void collide(GameEntity* other) override { collided_ = true; }
  void resize(float size);
  static void spawnAerolite();
  virtual void update(const GameEntity& aerolite, float delta_time) override;

 private:

  static void createB2Body(AerolitePhysicsComponent& aerolite);
  static void generateAeroliteShape(B2Vec2Vector& shape, float size);
  static void generateAeroliteShape(B2Vec2Vector& shape, float size, unsigned int sides);
  void split();
  void transformRenderShape();

  static constexpr float kMinSize_ {0.5f};
  static constexpr unsigned int kMaxSides_ {b2_maxPolygonVertices};
  static constexpr unsigned int kMinSides_ {6u};
  static constexpr unsigned int kScore_ {1000u};

  AeroliteGraphicsComponent* graphics_ {nullptr};
  /**
   * @brief This is used to know when an aerolite has left the screen.
   */
  b2AABB aabb_ {};
};

} // namespace ktp