#ifndef AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_
#define AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_

#include "palette.hpp"

#include <SDL.h>
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class GameEntity;
class SDL2_Renderer;
class Particle;

class GraphicsComponent {
 public:
  virtual ~GraphicsComponent() {}
  virtual void update(const GameEntity&, const SDL2_Renderer&) = 0;
 protected:
  FPointsVector render_shape_ {};
};

class AeroliteGraphicsComponent: public GraphicsComponent {
 public:
  virtual void update(const GameEntity& aerolite, const SDL2_Renderer& renderer) override;
 private:
  friend class AerolitePhysicsComponent;
  static constexpr SDL_Color kDefaultColor_ {Colors::orange};
};

class EmitterGraphicsComponent: public GraphicsComponent {
 public:
  virtual void update(const GameEntity& emitter, const SDL2_Renderer& renderer) override;
  ~EmitterGraphicsComponent() { delete[] particles_pool_; }
 private:
  friend class EmitterPhysicsComponent;
  SDL_BlendMode blend_mode_ {};
  Particle* particles_pool_ {nullptr};
  unsigned int particles_pool_size_ {0};
};

class PlayerGraphicsComponent: public GraphicsComponent {
 public:
  virtual void update(const GameEntity& player, const SDL2_Renderer& renderer) override;
 private:
  friend class PlayerPhysicsComponent;
  static constexpr SDL_Color kDefaultColor_ {Colors::white};
  FPointsVector render_flame_shape_ {};
  bool thrusting_ {false};
};

class ProjectileGraphicsComponent: public GraphicsComponent {
 public:
  virtual void update(const GameEntity& projectile, const SDL2_Renderer& renderer) override;
 private:
  friend class ProjectilePhysicsComponent;
  static constexpr SDL_Color kDefaultColor_ {Colors::copper_green};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GRAPHICS_COMPONENT_HPP_