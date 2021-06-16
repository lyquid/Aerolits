#ifndef AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_

#include "palette.hpp"

namespace ktp {

class GameObject;
class Player;
class SDL2_Renderer;

class GraphicsComponent {
 public:
  virtual ~GraphicsComponent() {}
  virtual void update(const Player&, SDL2_Renderer&) = 0;
};

class PlayerGraphicsComponent : public GraphicsComponent {
 public:
  virtual void update(const Player& player, SDL2_Renderer& renderer);
 private:
  static constexpr SDL_Color kDefaultPlayerColor_ {Colors::white};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_