#ifndef AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_

#include "aerolite.hpp"
#include "background.hpp"
#include "emitter.hpp"
#include "graphics_component.hpp"
#include "input_component.hpp"
#include "physics_component.hpp"
#include "player.hpp"
#include "projectile.hpp"
#include "../../kuge/kuge.hpp"
#include <memory>
#include <utility> // std::move std::exchange

namespace ktp {

using EntityId = unsigned int;
using Graphics = std::unique_ptr<GraphicsComponent>;
using Input    = std::unique_ptr<InputComponent>;
using Physics  = std::unique_ptr<PhysicsComponent>;

enum class GameEntities {
  Aerolite,
  Background,
  Player,
  PlayerDemo,
  Projectile,
  count
};

class SDL2_Renderer;

class GameEntity {

  friend class DemoState;
  friend class PlayingState;
  friend class InputComponent;
  friend class AerolitePhysicsComponent;
  friend class EmitterPhysicsComponent;
  friend class PlayerPhysicsComponent;

 public:

  GameEntity(const GameEntity& other) = delete;
  GameEntity(GameEntity&& other):
   id_(other.id_),
   type_(other.type_) {
    *this = std::move(other);
    ++count_;
  }
  ~GameEntity() { --count_; }

  GameEntity& operator=(const GameEntity& other) = delete;
  GameEntity& operator=(GameEntity&& other) noexcept {
    if (this != &other) {
      delta_    = other.delta_;
      graphics_ = std::exchange(other.graphics_, nullptr);
      input_    = std::exchange(other.input_, nullptr);
      physics_  = std::exchange(other.physics_, nullptr);
    }
    return *this;
  }

  static GameEntity createEntity(GameEntities type) {
    GameEntity entity {type};
    switch (type) {
      case GameEntities::Aerolite:
        entity.graphics_ = std::make_unique<AeroliteGraphicsComponent>();
        entity.physics_  = std::make_unique<AerolitePhysicsComponent>(static_cast<AeroliteGraphicsComponent*>(entity.graphics_.get()));
        break;
      case GameEntities::Background:
        entity.graphics_ = std::make_unique<BackgroundGraphicsComponent>();
        entity.physics_  = std::make_unique<BackgroundPhysicsComponent>(static_cast<BackgroundGraphicsComponent*>(entity.graphics_.get()));
        break;
      case GameEntities::Player:
        entity.graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity.physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(entity.graphics_.get()));
        entity.input_    = std::make_unique<PlayerInputComponent>(static_cast<PlayerPhysicsComponent*>(entity.physics_.get()));
        break;
      case GameEntities::PlayerDemo:
        entity.graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity.physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(entity.graphics_.get()));
        entity.input_    = std::make_unique<DemoInputComponent>(static_cast<PlayerPhysicsComponent*>(entity.physics_.get()));
        break;
      case GameEntities::Projectile:
        entity.graphics_ = std::make_unique<ProjectileGraphicsComponent>();
        entity.physics_  = std::make_unique<ProjectilePhysicsComponent>(static_cast<ProjectileGraphicsComponent*>(entity.graphics_.get()));
        break;
      case GameEntities::count:
        [[fallthrought]]
      default:
        // boom!
        break;
    }
    return std::move(entity);
  }

  inline static auto count() { return count_; }

  inline static auto currentId() { return current_id_; }

  inline void draw(const SDL2_Renderer& renderer) const {
    if (graphics_) graphics_->update(*this, renderer);
  }

  inline EntityId id() const { return id_; }

  inline void update(float delta_time) {
    if (input_) input_->update(*this, delta_time);
    if (physics_) physics_->update(*this, delta_time);
  }

 private:

  GameEntity(GameEntities type):
   id_(getId()),
   type_(type) {
    ++count_;
  }

  inline static EntityId getId() { return current_id_++; }

  // defined in game.cpp
  static EntityId count_;
  static EntityId current_id_;

  const EntityId     id_;
  const GameEntities type_;
  SDL_FPoint      delta_ {}; // i dont like this here
  Graphics        graphics_ {nullptr};
  Input           input_ {nullptr};
  Physics         physics_ {nullptr};
  //kuge::EventBus& event_bus_;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_