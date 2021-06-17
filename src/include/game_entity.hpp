#ifndef AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_

#include "graphics_component.hpp"
#include "input_component.hpp"
#include "physics_component.hpp"
#include "../../sdl2_wrappers/sdl2_renderer.hpp"
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

class GameEntity {
 public:
  GameEntity(kuge::EventBus& event_bus, GameEntities type): event_bus_(event_bus) {
    switch (type) {
      case GameEntities::Aerolite:
        /* graphics_ = std::make_unique<AeroliteGraphicsComponent>();
        physics_ = std::make_unique<AerolitePhysicsComponent>(graphics_.get()); */
        break;
      case GameEntities::Player:
        graphics_ = std::make_unique<PlayerGraphicsComponent>();
        physics_ = std::make_unique<PlayerPhysicsComponent>(graphics_.get());
        input_ = std::make_unique<PlayerInputComponent>(physics_.get());
        break;
      case GameEntities::PlayerDemo:
        graphics_ = std::make_unique<PlayerGraphicsComponent>();
        physics_ = std::make_unique<PlayerPhysicsComponent>(graphics_.get());
        input_ = std::make_unique<DemoInputComponent>(physics_.get());
        break;
      default:
        // boom!
        break;
    }
  }

  void draw(const SDL2_Renderer& renderer) const {
    if (graphics_) graphics_->update(*this, renderer);
  }

  void update(SDL2_Renderer& renderer, float delta_time) {
    if (input_) input_->update(*this, delta_time);
    if (physics_) physics_->update(*this, delta_time);
  }

  inline void getDelta(float& x, float& y) const { x = delta_.x; y = delta_.y; }
  inline SDL_FPoint getDelta() const { return delta_; }
  inline void setDelta(const SDL_FPoint& delta) { delta_ = delta; }
  inline void setDelta(float x, float y) { delta_.x = x; delta_.y = y; }

 private:
  friend class InputComponent;

  SDL_FPoint delta_ {};
  Graphics graphics_ {nullptr};
  Input    input_ {nullptr};
  Physics  physics_ {nullptr};
  kuge::EventBus& event_bus_;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_ENTITY_HPP_