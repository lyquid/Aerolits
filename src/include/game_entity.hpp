#ifndef AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_

#include "../kuge/kuge.hpp"
#include "aerolite.hpp"
#include "background.hpp"
#include "emitter.hpp"
#include "explosion.hpp"
#include "graphics_component.hpp"
#include "input_component.hpp"
#include "object_pool.hpp"
#include "physics_component.hpp"
#include "player.hpp"
#include "projectile.hpp"
#include <map>
#include <memory>
#include <utility> // std::move std::exchange

namespace ktp {

enum class EntityTypes {
  Undefined,
  Aerolite,
  Background,
  Emitter,
  Explosion,
  Player,
  PlayerDemo,
  Projectile,
  count
};

using EntitiesCount = std::map<EntityTypes, std::size_t>;
using EntitiesPool  = IndexedObjectPool<GameEntity>;
using Graphics      = std::unique_ptr<GraphicsComponent>;
using Input         = std::unique_ptr<InputComponent>;
using Physics       = std::unique_ptr<PhysicsComponent>;

class GameEntity {

  // this friend is needed b/c the constructor is private
  friend class IndexedObjectPool<GameEntity>;
  friend struct IndexedPoolUnit<GameEntity>;

 public:

  GameEntity(const GameEntity& other) = delete;
  GameEntity(GameEntity&& other) noexcept: type_(other.type_) { *this = std::move(other); }
  ~GameEntity() { --entities_count_[type_]; }

  GameEntity& operator=(const GameEntity& other) = delete;
  GameEntity& operator=(GameEntity&& other) noexcept {
    if (this != &other) {
      deactivate_ = other.deactivate_;
      graphics_   = std::exchange(other.graphics_, nullptr);
      input_      = std::exchange(other.input_, nullptr);
      physics_    = std::exchange(other.physics_, nullptr);
      type_       = other.type_;
    }
    return *this;
  }

  /**
   * @return The status of the deactivate flag.
   */
  auto canBeDeactivated() const { return deactivate_; }

  /**
   * @brief Clears all the GameEntities in the object pool by calling reset()
   *        on them. Also resets the counters to 0.
   */
  static void clear() {
    for (auto i = 0u; i < game_entities_.capacity(); ++i) {
      game_entities_[i].reset();
    }
    game_entities_.clear();
    entities_count_.clear();
  }

  /**
   * @brief "Creates" a GameEntity object.
   * @param type The type of GameEntity desired.
   * @return A pointer to the newly created GameEntity or nullptr.
   */
  static auto createEntity(EntityTypes type) {
    const auto entity {game_entities_.activate()};
    if (!entity) return entity;
    entity->deactivate_ = false;
    entity->type_ = type;
    switch (entity->type_) {
      case EntityTypes::Aerolite:
        entity->graphics_ = std::make_unique<AeroliteGraphicsComponent>();
        entity->physics_  = std::make_unique<AerolitePhysicsComponent>(entity, static_cast<AeroliteGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Background:
        entity->graphics_ = std::make_unique<BackgroundGraphicsComponent>();
        entity->physics_  = std::make_unique<BackgroundPhysicsComponent>(entity, static_cast<BackgroundGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Emitter:
        entity->graphics_ = std::make_unique<EmitterGraphicsComponent>();
        entity->physics_  = std::make_unique<EmitterPhysicsComponent>(entity, static_cast<EmitterGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Explosion:
        entity->graphics_ = std::make_unique<ExplosionGraphicsComponent>();
        entity->physics_  = std::make_unique<ExplosionPhysicsComponent>(entity, static_cast<ExplosionGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Player:
        entity->graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity->physics_  = std::make_unique<PlayerPhysicsComponent>(entity, static_cast<PlayerGraphicsComponent*>(entity->graphics_.get()));
        entity->input_    = std::make_unique<PlayerInputComponent>(static_cast<PlayerPhysicsComponent*>(entity->physics_.get()));
        break;
      case EntityTypes::PlayerDemo:
        entity->graphics_ = std::make_unique<PlayerGraphicsComponent>();
        entity->physics_  = std::make_unique<PlayerPhysicsComponent>(entity, static_cast<PlayerGraphicsComponent*>(entity->graphics_.get()));
        entity->input_    = std::make_unique<DemoInputComponent>(static_cast<PlayerPhysicsComponent*>(entity->physics_.get()));
        break;
      case EntityTypes::Projectile:
        entity->graphics_ = std::make_unique<ProjectileGraphicsComponent>();
        entity->physics_  = std::make_unique<ProjectilePhysicsComponent>(entity, static_cast<ProjectileGraphicsComponent*>(entity->graphics_.get()));
        break;
      case EntityTypes::Undefined:
        break;
      case EntityTypes::count:
      default:
        entity->type_ = EntityTypes::Undefined;
        break;
    }
    ++entities_count_[entity->type_];
    return entity;
  }

  /**
   * @return The number of GameEntities currently active.
   */
  static auto count() { return game_entities_.activeCount(); }

  /**
   * @brief Sets the deactivate flag to true.
   */
  void deactivate() { deactivate_ = true; }

  /**
   * @brief Uses the graphics component to draw something hopefully ressembling
   *         what the user wants.
   * @param renderer A renderer to draw things on.
   */
  void draw() const {
    if (graphics_) graphics_->update(*this);
  }

  /**
   * @brief Use this to get the number of entities active of a given type.
   * @param type The type of entity to look for.
   * @return The number of active entities of the type requested.
   */
  static auto entitiesCount(EntityTypes type) { return entities_count_[type]; }

  /**
   * @brief Resets the GameEntity and sets it the first available in the pool.
   * @param index The index of the GameEntity in the pool.
   */
  void free(std::size_t index) {
    reset();
    game_entities_.deactivate(index);
  }

  /**
   * @return A pointer to the graphics component.
   */
  auto graphics() const { return graphics_.get(); }

  /**
   * @return A pointer to the input component.
   */
  auto input() const { return input_.get(); }

  /**
   * @return A pointer to the physics component.
   */
  auto physics() const { return physics_.get(); }

  /**
   * @return The type of the GameEntity.
   */
  auto type() const { return type_; }

  /**
   * @brief Updates the input and physics components of the GameEntity.
   * @param delta_time A time that goes between gamma_time and epsilon_time.
   */
  void update(float delta_time) {
    if (input_) input_->update(*this, delta_time);
    if (physics_) physics_->update(*this, delta_time);
  }

  /**
   * @brief All the GameEntities are in this pool.
   */
  static EntitiesPool game_entities_;

  // defined in game.cpp
  static kuge::EventBus* event_bus_;

 private:

  GameEntity(EntityTypes type = EntityTypes::Undefined) noexcept: type_(type) {
    ++entities_count_[type_];
  }

  /**
   * @brief Puts the GameEntity at Undefined type/state and resets all the
   *  components to nullptr, causing the smart pointer to invoke the deleter,
   *  freeing the memory.
   */
  void reset() {
    deactivate_ = false;
    graphics_ = nullptr;
    input_    = nullptr;
    physics_  = nullptr;
    --entities_count_[type_];
    type_ = EntityTypes::Undefined;
    ++entities_count_[type_];
  }

  // defined in game.cpp
  static EntitiesCount entities_count_;

  bool        deactivate_ {false};
  Graphics    graphics_ {nullptr};
  Input       input_ {nullptr};
  Physics     physics_ {nullptr};
  EntityTypes type_;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_
