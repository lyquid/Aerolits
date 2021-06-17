#ifndef AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_

#include <box2d/box2d.h>
#include <SDL.h>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class GameEntity;
class GraphicsComponent;

class PhysicsComponent {
 public:
  virtual ~PhysicsComponent() {}
  virtual void update(const GameEntity&, float) = 0;
  static void setScreenSize(const SDL_FPoint& screen_size);
  static void setWorld(b2World* world) { world_ = world; }
 protected:
  friend class InputComponent;
  static SDL_FPoint b2_screen_size_;
  static b2World* world_;
  b2Body* body_ {nullptr};
};

class PlayerPhysicsComponent : public PhysicsComponent {
 public:
  PlayerPhysicsComponent(GraphicsComponent* graphics);
  virtual void update(const GameEntity& player, float delta_time) override;
 private:
  static void generatePlayerShape(FPointsVector& shape, float size);
  void checkWrap();
  void setBox2D();
  void transformRenderShape();

  static constexpr float kDefaultPlayerSize_ {1.2f};
  GraphicsComponent* graphics_ {nullptr};
  FPointsVector shape_;
  float size_ {kDefaultPlayerSize_};
};

/* class AerolitePhysicsComponent : public PhysicsComponent {
 public:
  virtual void update(const GameEntity& aerolite, float delta_time) override;
}; */

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_