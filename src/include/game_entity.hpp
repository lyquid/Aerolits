#ifndef AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_

#include "aerolite.hpp"
#include "emitter.hpp"
#include "graphics_component.hpp"
#include "input_component.hpp"
#include "physics_component.hpp"
#include "player.hpp"
#include "projectile.hpp"
#include "../../kuge/kuge.hpp"
#include <memory>

namespace ktp {

using Graphics = std::unique_ptr<GraphicsComponent>;
using Input    = std::unique_ptr<InputComponent>;
using Physics  = std::unique_ptr<PhysicsComponent>;

enum class GameEntities {
  Aerolite,
  Emitter,
  Player,
  PlayerDemo,
  Projectile,
  count
};

class SDL2_Renderer;

class GameEntity {
 public:
  GameEntity() = delete;
  GameEntity(/* kuge::EventBus& event_bus,  */GameEntities type)/* : event_bus_(event_bus)  */{
    switch (type) {
      case GameEntities::Aerolite:
        graphics_ = std::make_unique<AeroliteGraphicsComponent>();
        physics_  = std::make_unique<AerolitePhysicsComponent>(static_cast<AeroliteGraphicsComponent*>(graphics_.get()));
        break;
      case GameEntities::Emitter:
        graphics_ = std::make_unique<EmitterGraphicsComponent>();
        physics_  = std::make_unique<EmitterPhysicsComponent>(static_cast<EmitterGraphicsComponent*>(graphics_.get()));
        break;
      case GameEntities::Player:
        graphics_ = std::make_unique<PlayerGraphicsComponent>();
        physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(graphics_.get()));
        input_    = std::make_unique<PlayerInputComponent>(static_cast<PlayerPhysicsComponent*>(physics_.get()));
        break;
      case GameEntities::PlayerDemo:
        graphics_ = std::make_unique<PlayerGraphicsComponent>();
        physics_  = std::make_unique<PlayerPhysicsComponent>(static_cast<PlayerGraphicsComponent*>(graphics_.get()));
        input_    = std::make_unique<DemoInputComponent>(static_cast<PlayerPhysicsComponent*>(physics_.get()));
        break;
      case GameEntities::Projectile:
        graphics_ = std::make_unique<ProjectileGraphicsComponent>();
        physics_  = std::make_unique<ProjectilePhysicsComponent>(static_cast<ProjectileGraphicsComponent*>(graphics_.get()));
        break;
      default:
        // boom!
        break;
    }
  }

  inline void draw(const SDL2_Renderer& renderer) const {
    if (graphics_) graphics_->update(*this, renderer);
  }

  inline void update(float delta_time) {
    if (input_) input_->update(*this, delta_time);
    if (physics_) physics_->update(*this, delta_time);
  }

 private:

  friend class DemoState;
  friend class PlayingState;
  friend class InputComponent;
  friend class AerolitePhysicsComponent;
  friend class EmitterPhysicsComponent;
  friend class PlayerPhysicsComponent;

  SDL_FPoint      delta_ {};
  Graphics        graphics_ {nullptr};
  Input           input_ {nullptr};
  Physics         physics_ {nullptr};
  //kuge::EventBus& event_bus_;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_