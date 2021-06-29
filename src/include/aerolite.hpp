#pragma once

#include "graphics_component.hpp"
#include "palette.hpp"
#include "physics_component.hpp"
#include <box2d/box2d.h>
#include <SDL.h>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class GameEntity;
class SDL2_Renderer;

class AeroliteGraphicsComponent: public GraphicsComponent {
  friend class AerolitePhysicsComponent;
 public:
  virtual void update(const GameEntity& aerolite, const SDL2_Renderer& renderer) override;
 private:
  static constexpr SDL_Color kDefaultColor_ {Colors::orange};
};

class AerolitePhysicsComponent: public PhysicsComponent {
 public:
  AerolitePhysicsComponent(AeroliteGraphicsComponent* graphics);
  inline float getMaxSpeed() const { return kMaxSpeed_; }
  virtual void setPosition(const SDL_FPoint& pos) override {}
  static GameEntity spawnAerolite();
  virtual void update(const GameEntity& aerolite, float delta_time) override;
 private:
  static void generateAeroliteShape(FPointsVector& shape, float size);
  void transformRenderShape();
  static constexpr float kKgPerMeter2_ {20.f};
  static constexpr float kMaxRotationSpeed_ {2.f};
  static constexpr float kMaxSize_ {5.f};
  static constexpr float kMaxSpeed_ {10.f};
  AeroliteGraphicsComponent* graphics_ {nullptr};
  b2AABB aabb_ {};
};

} // namespace ktp