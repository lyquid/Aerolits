#ifndef AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_

#include <SDL.h>
#include <vector>

namespace ktp {

class GameEntity;

class GraphicsComponent {

 public:

  // virtual ~GraphicsComponent() {}
  // inline auto& renderShape() { return render_shape_; }
  // virtual void update(const GameEntity&, const SDL2_Renderer&) = 0;
  virtual ~GraphicsComponent() {}
  virtual void update(const GameEntity&) = 0;

 protected:

  // FPointsVector render_shape_ {};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
