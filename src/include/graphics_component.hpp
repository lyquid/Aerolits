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
  inline auto& renderShape() { return render_shape_; }
  virtual void update(const GameEntity&, const SDL2_Renderer&) = 0;

 protected:

  FPointsVector render_shape_ {};
};

class GLGraphicsComponent {

 public:

  virtual ~GLGraphicsComponent() {}
  virtual void update(const GameEntity&) = 0;

};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
