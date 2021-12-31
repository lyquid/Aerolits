#ifndef AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_

#include <SDL.h>
#include <vector>

namespace ktp {

class GameEntity;

class GraphicsComponent {

 public:

  virtual ~GraphicsComponent() {}
  virtual void update(const GameEntity&) = 0;

};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
