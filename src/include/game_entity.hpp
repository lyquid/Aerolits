#ifndef AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_

#include "graphics_component.hpp"
#include "input_component.hpp"
#include "physics_component.hpp"
#include "../../kuge/kuge.hpp"
#include <memory>

namespace ktp {

using Graphics = std::unique_ptr<GraphicsComponent>;
using Input    = std::unique_ptr<InputComponent>;
using Physics  = std::unique_ptr<PhysicsComponent>;

enum class GameEntities {
  Aerolite,
  Player,
  PlayerDemo,
  count
};

class SDL2_Renderer;

class GameEntity {
 public:

  GameEntity(/* kuge::EventBus& event_bus,  */GameEntities type)/* : event_bus_(event_bus)  */{
    switch (type) {
      case GameEntities::Aerolite:
        graphics_ = std::make_unique<AeroliteGraphicsComponent>();
        physics_  = std::make_unique<AerolitePhysicsComponent>(static_cast<AeroliteGraphicsComponent*>(graphics_.get())); 
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
      default:
        // boom!
        break;
    }
  }

  inline void draw(const SDL2_Renderer& renderer) const {
    if (graphics_) graphics_->update(*this, renderer);
  }

  void update(float delta_time) {
    if (input_) input_->update(*this, delta_time);
    if (physics_) physics_->update(*this, delta_time);
  }

 private:

  friend class DemoState;
  friend class PlayingState;
  friend class InputComponent;
  friend class AerolitePhysicsComponent;

  SDL_FPoint      delta_ {};
  Graphics        graphics_ {nullptr};
  Input           input_ {nullptr};
  Physics         physics_ {nullptr};
  //kuge::EventBus& event_bus_;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_