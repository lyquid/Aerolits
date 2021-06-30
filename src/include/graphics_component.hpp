#ifndef AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_

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

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_