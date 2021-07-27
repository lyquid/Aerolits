#ifndef AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_

#include "aerolite.hpp"
#include "background.hpp"
#include "emitter.hpp"
#include "graphics_component.hpp"
#include "input_component.hpp"
#include "object_pool.hpp"
#include "physics_component.hpp"
#include "player.hpp"
#include "projectile.hpp"
#include "../../kuge/kuge.hpp"
#include <memory>
#include <utility> // std::move std::exchange

namespace ktp {

using EntitiesPool = ObjectPool<GameEntity>;
using EntityId = std::size_t;
using Graphics = std::unique_ptr<GraphicsComponent>;
using Input    = std::unique_ptr<InputComponent>;
using Physics  = std::unique_ptr<PhysicsComponent>;

enum class EntityTypes {
  Undefined,
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
  friend class AerolitePhysicsComponent;
  friend class EmitterPhysicsComponent;
  friend class PlayerPhysicsComponent;
  friend class ObjectPool<GameEntity>;

 public:

  GameEntity(const GameEntity& other) = delete;
  GameEntity(GameEntity&& other) noexcept { *this = std::move(other); }
  ~GameEntity() { if (type_ == EntityTypes::Aerolite) --aerolite_count_; }

  GameEntity& operator=(const GameEntity& other) = delete;
  GameEntity& operator=(GameEntity&& other) noexcept {
    if (this != &other) {
      graphics_ = std::exchange(other.graphics_, nullptr);
      input_    = std::exchange(other.input_, nullptr);
      physics_  = std::exchange(other.physics_, nullptr);
      type_     = other.type_;
    }
    return *this;
  }

  /**
   * @brief "Creates" a GameEntity object.
   * @param type The type of GameEntity desired.
   * @return A pointer to the newly created GameEntity or nullptr.
   */
  static auto createEntity(EntityTypes type) {
    const auto entity {game_entities_.create()};
    if (!entity) return entity;
    entity->type_ = type;
    switch (entity->type_) {
      case EntityTypes::Aerolite:
        entity->graphics_ = std::make_unique<AeroliteGraphicsComponent>();
        entity->physics_  = std::make_unique<AerolitePhysicsComponent>(static_cast<AeroliteGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Background:
        entity->graphics_ = std::make_unique<BackgroundGraphicsComponent>();
        entity->physics_  = std::make_unique<BackgroundPhysicsComponent>(static_cast<BackgroundGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Player:
        entity->graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity->physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(entity->graphics_.get()));
        entity->input_    = std::make_unique<PlayerInputComponent>(static_cast<PlayerPhysicsComponent*>(entity->physics_.get()));
        break;
      case EntityTypes::PlayerDemo:
        entity->graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity->physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(entity->graphics_.get()));
        entity->input_    = std::make_unique<DemoInputComponent>(static_cast<PlayerPhysicsComponent*>(entity->physics_.get()));
        break;
      case EntityTypes::Projectile:
        entity->graphics_ = std::make_unique<ProjectileGraphicsComponent>();
        entity->physics_  = std::make_unique<ProjectilePhysicsComponent>(static_cast<ProjectileGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Undefined:
        break;
      case EntityTypes::count:
        [[fallthrought]]
      default:
        entity->type_ = EntityTypes::Undefined;
        break;
    }
    return entity;
  }

  /**
   * @return The number of GameEntities currently active.
   */
  inline static auto count() { return game_entities_.activeCount(); }

  /**
   * @brief Puts the GameEntity at Undefined type/state and resets all the
   *        components to nullptr.
   */
  void disable() {
    graphics_ = nullptr;
    input_    = nullptr;
    physics_  = nullptr;
    type_ = EntityTypes::Undefined;
  }

  /**
   * @brief Uses the graphics component to draw something hopefully ressembling
   *         what the user wants.
   * @param renderer A renderer to draw things on.
   */
  inline void draw(const SDL2_Renderer& renderer) const {
    if (graphics_) graphics_->update(*this, renderer);
  }

  /**
   * @return A pointer to the graphics component.
   */
  inline auto graphics() const { return graphics_.get(); }

  /**
   * @return A pointer to the input component.
   */
  inline auto input() const { return input_.get(); }

  /**
   * @return A pointer to the physics component.
   */
  inline auto physics() const { return physics_.get(); }

  /**
   * @brief Resets all the GameEntities in the object pool by calling disable()
   *        on them. Also resets the counters to 0.
   */
  static void reset() {
    for (auto i = 0u; i < game_entities_.size(); ++i) {
      game_entities_[i].object_.disable();
    }
    game_entities_.clear();
    aerolite_count_ = 0;
  }

  /**
   * @return The type of the GameEntity.
   */
  inline auto type() const { return type_; }

  /**
   * @brief Updates the input and physics components of the GameEntity.
   * @param delta_time A time that goes between gamma_time and epsilon_time.
   */
  inline void update(float delta_time) {
    if (input_) input_->update(*this, delta_time);
    if (physics_) physics_->update(*this, delta_time);
  }

  /**
   * @brief All the GameEntities are in this pool.
   */
  static EntitiesPool game_entities_;

 private:

  GameEntity() noexcept {}
  GameEntity(EntityTypes type) noexcept: type_(type) {}

  // defined in game.cpp
  static EntityId aerolite_count_;

  Graphics    graphics_ {nullptr};
  Input       input_ {nullptr};
  Physics     physics_ {nullptr};
  EntityTypes type_ {EntityTypes::Undefined};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_