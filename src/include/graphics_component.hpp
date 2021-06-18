#ifndef AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_

#include "palette.hpp"
#include <SDL.h>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class GameEntity;
class SDL2_Renderer;

class GraphicsComponent {
 public:
  virtual ~GraphicsComponent() {}
  virtual void update(const GameEntity&, const SDL2_Renderer&) = 0;
 protected:
  FPointsVector render_shape_ {};
};

class PlayerGraphicsComponent : public GraphicsComponent {
 public:
  virtual void update(const GameEntity& player, const SDL2_Renderer& renderer) override;
 private:
  friend class PlayerPhysicsComponent;
  static constexpr SDL_Color kDefaultPlayerColor_ {Colors::white};
  FPointsVector render_flame_shape_ {};
  bool thrusting_ {false};
};

class AeroliteGraphicsComponent : public GraphicsComponent {
 public:
  virtual void update(const GameEntity& aerolite, const SDL2_Renderer& renderer) override;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_