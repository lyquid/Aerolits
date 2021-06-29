#ifndef AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_

#include "box2d_scale.hpp"
#include "emitter_parser.hpp"
#include "../../sdl2_wrappers/sdl2_timer.hpp"
#include <box2d/box2d.h>
#include <SDL.h>
#include <memory>
#include <string>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class AeroliteGraphicsComponent;
class EmitterGraphicsComponent;
class EmitterType;
class GameEntity;
class PlayerGraphicsComponent;
class PlayerInputComponent;
class ProjectileGraphicsComponent;

class PhysicsComponent {
  friend class InputComponent;
 public:
  virtual ~PhysicsComponent() { world_->DestroyBody(body_); }
  inline bool canBeDeleted() const { return to_be_deleted_; }
  virtual void update(const GameEntity&, float) = 0;
  virtual void setPosition(const SDL_FPoint& pos) = 0;
  static void setScreenSize(const SDL_FPoint& screen_size) {
    b2_screen_size_.x = screen_size.x / kMetersToPixels;
    b2_screen_size_.y = screen_size.y / kMetersToPixels;
  }
  static void setWorld(b2World* world) { world_ = world; }
  inline b2Body* getBody() const { return body_; }
 protected:
  // defined in game.cpp
  static SDL_FPoint b2_screen_size_;
  static b2World* world_;
  //
  b2Body* body_ {nullptr};
  FPointsVector shape_ {};
  float size_ {};
  bool to_be_deleted_ {false};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_PHYSICS_COMPONENT_HPP_