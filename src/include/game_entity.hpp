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
// #include "../../kuge/kuge.hpp"
#include <memory>
#include <utility> // std::move std::exchange

namespace ktp {

using EntitiesPool = ObjectPool<GameEntity>;
using EntityId     = std::size_t;
using Graphics     = std::unique_ptr<GraphicsComponent>;
using Input        = std::unique_ptr<InputComponent>;
using Physics      = std::unique_ptr<PhysicsComponent>;

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

  // this friend is needed b/c the constructor is private
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
   * @return The number of Aerolite enitities active.
   */
  inline static auto aeroliteCount() { return aerolite_count_; }

  /**
   * @return The status of the deactivate flag.
   */
  inline auto canBeDeactivated() const { return deactivate_; }

  /**
   * @brief Clears all the GameEntities in the object pool by calling reset()
   *        on them. Also resets the counters to 0.
   */
  static void clear() {
    for (auto i = 0u; i < game_entities_.capacity(); ++i) {
      game_entities_[i].object_.reset();
    }
    game_entities_.clear();
    aerolite_count_ = 0;
  }

  /**
   * @brief "Creates" a GameEntity object.
   * @param type The type of GameEntity desired.
   * @return A pointer to the newly created GameEntity or nullptr.
   */
  static auto createEntity(EntityTypes type) {
    const auto entity {game_entities_.create()};
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
    return entity;
  }

  /**
   * @return The number of GameEntities currently active.
   */
  inline static auto count() { return game_entities_.activeCount(); }

  /**
   * @brief Uses the graphics component to draw something hopefully ressembling
   *         what the user wants.
   * @param renderer A renderer to draw things on.
   */
  inline void draw(const SDL2_Renderer& renderer) const {
    if (graphics_) graphics_->update(*this, renderer);
  }

  /**
   * @brief Resets the GameEntity and sets it the first available in the pool.
   * @param index The index of the GameEntity in the pool.
   */
  void free(std::size_t index) {
    reset();
    game_entities_.destroy(index);
  }

  /**
   * @return A pointer to the graphics component.
   */
  inline auto graphics() const { return graphics_.get(); }

  /**
   * @brief Increases (or decreases) the aerolites count.
   * @param inc The amount to increase by. Negative to decrease.
   */
  inline static void increaseAeroliteCount(int inc) { aerolite_count_ = aerolite_count_ + inc; }

  /**
   * @return A pointer to the input component.
   */
  inline auto input() const { return input_.get(); }

  /**
   * @return A pointer to the physics component.
   */
  inline auto physics() const { return physics_.get(); }

  /**
   * @brief Puts the GameEntity at Undefined type/state and resets all the
   *        components to nullptr.
   */
  void reset() {
    deactivate_ = false;
    graphics_ = nullptr;
    input_    = nullptr;
    physics_  = nullptr;
    type_ = EntityTypes::Undefined;
  }

  /**
   * @brief Sets the deactivate flag to true.
   */
  inline void toBeDeactivated() { deactivate_ = true; }

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

  GameEntity(EntityTypes type = EntityTypes::Undefined) noexcept: type_(type) {}

  // defined in game.cpp
  static EntityId aerolite_count_;

  bool        deactivate_ {false};
  Graphics    graphics_ {nullptr};
  Input       input_ {nullptr};
  Physics     physics_ {nullptr};
  EntityTypes type_;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_