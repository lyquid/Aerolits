#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "palette.hpp"
#include "physics_component.hpp"
#include "../sdl2_wrappers/sdl2_geometry.hpp"
#include <box2d/box2d.h>
#include <SDL.h>
#include <utility> // std::move std::exchange
#include <vector>

namespace ktp {

using B2Vec2Vector = std::vector<b2Vec2>;
using B2Line = Geometry::Line<b2Vec2>;

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

  inline void collide(const GameEntity* other) override { collided_ = true; }
  void reshape(float size);
  static GameEntity* spawnAerolite(const b2Vec2& where);
  static GameEntity* spawnMovingAerolite();
  virtual void update(const GameEntity& aerolite, float delta_time) override;
  inline auto worldManifold() { return &world_manifold_; }

 private:

  static void createB2Body(AerolitePhysicsComponent& aerolite);
  static void generateAeroliteShape(B2Vec2Vector& shape, float size);
  static void generateAeroliteShape(B2Vec2Vector& shape, float size, unsigned int sides);
  void split();
  void transformRenderShape();

  static constexpr float kMinSize_ {0.8f};
  static constexpr unsigned int kMaxSides_ {30u};
  static constexpr unsigned int kMinSides_ {25u};
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
