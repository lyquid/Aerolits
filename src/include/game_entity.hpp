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
#include <list>
#include <memory>
#include <utility> // std::move std::exchange

namespace ktp {

using EntityId = unsigned int;
using Graphics = std::unique_ptr<GraphicsComponent>;
using Input    = std::unique_ptr<InputComponent>;
using Physics  = std::unique_ptr<PhysicsComponent>;
using Entities = std::list<GameEntity>;

enum class EntityTypes {
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
    if (type_ == EntityTypes::Aerolite) ++aerolite_count_;
  }
  
  ~GameEntity() {
    --count_;
    if (type_ == EntityTypes::Aerolite) --aerolite_count_;
  }

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

  static auto createEntity(EntityTypes type) {
    GameEntity entity {type};
    switch (type) {
      case EntityTypes::Aerolite:
        entity.graphics_ = std::make_unique<AeroliteGraphicsComponent>();
        entity.physics_  = std::make_unique<AerolitePhysicsComponent>(static_cast<AeroliteGraphicsComponent*>(entity.graphics_.get()));
        break;
      case EntityTypes::Background:
        entity.graphics_ = std::make_unique<BackgroundGraphicsComponent>();
        entity.physics_  = std::make_unique<BackgroundPhysicsComponent>(static_cast<BackgroundGraphicsComponent*>(entity.graphics_.get()));
        break;
      case EntityTypes::Player:
        entity.graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity.physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(entity.graphics_.get()));
        entity.input_    = std::make_unique<PlayerInputComponent>(static_cast<PlayerPhysicsComponent*>(entity.physics_.get()));
        break;
      case EntityTypes::PlayerDemo:
        entity.graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity.physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(entity.graphics_.get()));
        entity.input_    = std::make_unique<DemoInputComponent>(static_cast<PlayerPhysicsComponent*>(entity.physics_.get()));
        break;
      case EntityTypes::Projectile:
        entity.graphics_ = std::make_unique<ProjectileGraphicsComponent>();
        entity.physics_  = std::make_unique<ProjectilePhysicsComponent>(static_cast<ProjectileGraphicsComponent*>(entity.graphics_.get()));
        break;
      case EntityTypes::count:
        [[fallthrought]]
      default:
        // boom!
        break;
    }
    game_entities_.push_back(std::move(entity));
    return &game_entities_.back();
  }

  inline static auto count() { return count_; }

  inline static auto currentId() { return current_id_; }

  inline void draw(const SDL2_Renderer& renderer) const {
    if (graphics_) graphics_->update(*this, renderer);
  }

  inline auto id() const { return id_; }

  static inline bool isPlayer(const GameEntity& entity) { return entity.type_ == EntityTypes::Player; }

  inline auto type() const { return type_; }

  inline void update(float delta_time) {
    if (input_) input_->update(*this, delta_time);
    if (physics_) physics_->update(*this, delta_time);
  }

  static Entities game_entities_;

 private:

  GameEntity(EntityTypes type):
   id_(getId()),
   type_(type) {
    ++count_;
    if (type_ == EntityTypes::Aerolite) ++aerolite_count_;
  }

  inline static EntityId getId() { return current_id_++; }

  // defined in game.cpp
  static EntityId count_;
  static EntityId current_id_;
  static EntityId aerolite_count_;

  const EntityId    id_;
  const EntityTypes type_;
  SDL_FPoint      delta_ {}; // i dont like this here
  Graphics        graphics_ {nullptr};
  Input           input_ {nullptr};
  Physics         physics_ {nullptr};
  //kuge::EventBus& event_bus_;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_