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
  static FPointsVector render_shape_;
};

class PlayerGraphicsComponent : public GraphicsComponent {
 public:
  virtual void update(const GameEntity& player, const SDL2_Renderer& renderer) override;
 private:
  static constexpr SDL_Color kDefaultPlayerColor_ {Colors::white};
};

class AeroliteGraphicsComponent : public GraphicsComponent {
 public:
  virtual void update(const GameEntity& aerolite, const SDL2_Renderer& renderer) override;
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_