#pragma once

#include "graphics_component.hpp"
#include "palette.hpp"
#include "physics_component.hpp"
#include "../../sdl2_wrappers/sdl2_timer.hpp"
#include <SDL.h>
#include <memory>
#include <utility> // std::move
#include <vector>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;

class EmitterGraphicsComponent;
class EmitterPhysicsComponent;
class GameEntity;
class SDL2_Renderer;

class PlayerGraphicsComponent: public GraphicsComponent {
  friend class PlayerPhysicsComponent;
 public:
  PlayerGraphicsComponent() noexcept;
  virtual void update(const GameEntity& player, const SDL2_Renderer& renderer) override;
 private:
  static constexpr SDL_Color kDefaultColor_ {Colors::white};
  FPointsVector render_flame_shape_ {};
  bool          thrusting_ {false};
  std::unique_ptr<EmitterGraphicsComponent> exhaust_emitter_ {nullptr};
};

class DemoInputComponent: public InputComponent {
 public:
  DemoInputComponent(PlayerPhysicsComponent* physics): InputComponent(physics) {}
  virtual void update(GameEntity& player, float delta_time) override;
 private:
  static constexpr int kThrustingInterval_ {5000};
  bool   thrust_ {};
  Uint32 thrusting_timer_ {SDL2_Timer::getSDL2Ticks()};
};

class PlayerInputComponent: public InputComponent {
 public:
  PlayerInputComponent(PlayerPhysicsComponent* physics): InputComponent(physics) {}
  virtual void update(GameEntity& player, float delta_time) override;
 private:
};

class PlayerPhysicsComponent: public PhysicsComponent {

  friend class InputComponent;

 public:

  PlayerPhysicsComponent(PlayerGraphicsComponent* graphics) noexcept;
  PlayerPhysicsComponent(const PlayerPhysicsComponent& other) = delete;
  PlayerPhysicsComponent(PlayerPhysicsComponent&& other) { *this = std::move(other); }

  PlayerPhysicsComponent& operator=(const PlayerPhysicsComponent& other) = delete;
  PlayerPhysicsComponent& operator=(PlayerPhysicsComponent&& other) noexcept;

  virtual void setPosition(const SDL_FPoint& pos) override {}
  virtual void update(const GameEntity& player, float delta_time) override;

 private:

  static void generatePlayerShape(FPointsVector& shape, FPointsVector& flame_shape, float size);
  void checkWrap();
  void setBox2D();
  void transformRenderShape();

  static constexpr float kDefaultPlayerSize_ {1.2f};
  PlayerGraphicsComponent* graphics_ {nullptr};
  FPointsVector flame_shape_ {};
  bool thrusting_ {false};
  // flame stuff
  static constexpr float kDefaultFlameGrowthFactor_ {0.02f};
  static constexpr float kDefaultFlameMaxLength_ {kDefaultPlayerSize_};
  static constexpr float kDefaultFlameMinLength_ {kDefaultPlayerSize_ * 0.4f};
  float flame_growth_factor_ {kDefaultFlameGrowthFactor_};
  float flame_max_lenght_ {kDefaultFlameMaxLength_};
  // emitter stuff
  std::unique_ptr<EmitterPhysicsComponent> exhaust_emitter_ {nullptr};
};

} // namespace ktp