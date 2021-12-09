#ifndef AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_

#include "box2d_utils.hpp"
#include <box2d/box2d.h>
#include <SDL.h>
#include <vector>

namespace ktp {

using B2Vec2Vector = std::vector<b2Vec2>;

class PhysicsComponent {

 public:

  virtual ~PhysicsComponent() { if (body_) world_->DestroyBody(body_); }

  inline auto body() const { return body_; }
  inline auto collided() const { return collided_; }
  inline auto delta() const { return delta_; }
  inline auto owner() const { return owner_; }
  inline void setBody(b2Body* body) { body_ = body; }
  inline auto& shape() { return shape_; }
  inline auto size() const { return size_; }

  virtual void collide(const GameEntity*) = 0;
  virtual void update(const GameEntity&, float) = 0;

  static inline auto& b2ScreenSize() { return b2_screen_size_; }
  static inline void setScreenSize(const SDL_FPoint& screen_size) {
    b2_screen_size_.x = screen_size.x * kPixelsToMeters;
    b2_screen_size_.y = screen_size.y * kPixelsToMeters;
  }
  static inline void setWorld(b2World* world) { world_ = world; }

 protected:

  // defined in game.cpp
  static SDL_FPoint b2_screen_size_;
  static b2World*   world_;

  b2Body*      body_ {nullptr};
  bool         collided_ {false};
  SDL_FPoint   delta_ {};
  GameEntity*  owner_ {nullptr};
  B2Vec2Vector shape_ {};
  float        size_ {};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_
