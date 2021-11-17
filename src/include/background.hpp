#pragma once

#include "graphics_component.hpp"
#include "palette.hpp"
#include "physics_component.hpp"
#include <SDL.h>
#include <array>
#include <utility> // std::move
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class GameEntity;
class SDL2_Renderer;

struct Star {
  SDL_Color  color_{};
  SDL_FPoint delta_{};
  SDL_FPoint position_{};
};

class BackgroundGraphicsComponent: public GraphicsComponent {
 public:
  inline auto& stars() { return stars_; }
  inline auto& starColors() const { return star_colors_; }
  virtual void update(const GameEntity& background, const SDL2_Renderer& renderer) override;
 private:
  static inline SDL_Rect shapeToRect(const FPointsVector& render_shape);
  static constexpr SDL_Color kBackgroundColor_ {Colors::black};
  std::vector<Star> stars_ {};
  const std::array<SDL_Color, 4> star_colors_ {Colors::purple,    Colors::copper_green,
                                               Colors::turquoise, Colors::yellow};
};

class BackgroundPhysicsComponent: public PhysicsComponent {

 public:

  BackgroundPhysicsComponent(GameEntity* owner, BackgroundGraphicsComponent* graphics) noexcept;
  BackgroundPhysicsComponent(const BackgroundPhysicsComponent& other) = delete;
  BackgroundPhysicsComponent(BackgroundPhysicsComponent&& other) { *this = std::move(other); }

  BackgroundPhysicsComponent& operator=(const BackgroundPhysicsComponent& other) = delete;
  BackgroundPhysicsComponent& operator=(BackgroundPhysicsComponent&& other) noexcept;

  virtual void collide(const GameEntity* other) override {}
  virtual void update(const GameEntity& background, float delta_time) override;

 private:

  void generateStars();
  static constexpr float kXtraSpace_ {50.f};
  BackgroundGraphicsComponent* graphics_ {nullptr};
};

} // end namespace ktp
