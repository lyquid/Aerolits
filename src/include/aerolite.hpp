#pragma once

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
  static constexpr SDL_Color kDefaultColor_ {Colors::orange};
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
  inline float getMaxSpeed() const { return kMaxSpeed_; }
  void resize(float size);
  virtual void setPosition(const SDL_FPoint& pos) override {}
  static void spawnAerolite();
  virtual void update(const GameEntity& aerolite, float delta_time) override;

 private:

  static void createB2Body(AerolitePhysicsComponent& aerolite);
  static void generateAeroliteShape(B2Vec2Vector& shape, float size);
  static void generateAeroliteShape(B2Vec2Vector& shape, float size, unsigned int sides);
  void split();
  void transformRenderShape();

  static constexpr float kKgPerMeter2_ {20.f};
  static constexpr float kMaxRotationSpeed_ {2.f};
  static constexpr float kMaxSize_ {3.f};
  static constexpr float kMinSize_ {0.5f};
  static constexpr float kMaxSpeed_ {10.f};
  static constexpr unsigned int kMaxSides_ {b2_maxPolygonVertices};
  static constexpr unsigned int kMinSides_ {6u};

  AeroliteGraphicsComponent* graphics_ {nullptr};
  b2AABB aabb_ {};
};

} // namespace ktp