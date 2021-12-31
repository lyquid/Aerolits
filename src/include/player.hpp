#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "physics_component.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include "../sdl2_wrappers/sdl2_timer.hpp"
#include <utility> // std::move
#include <memory>

namespace ktp {

using FPointsVector = std::vector<SDL_FPoint>;
using B2Vec2Vector  = std::vector<b2Vec2>;

class EmitterGraphicsComponent;
class EmitterPhysicsComponent;
class GameEntity;

class PlayerGraphicsComponent: public GraphicsComponent {
  friend class PlayerPhysicsComponent;
 public:
  PlayerGraphicsComponent() noexcept;
  virtual void update(const GameEntity& player) override;
 private:
  void generateOpenGLStuff(float size);
  b2Color color_ {SDL2ColorToB2Color(ConfigParser::player_config.color_)};
  VAO vao_ {};
  VBO vertices_ {};
  EBO vertices_indices_ {};
  ShaderProgram shader_;
  glm::mat4 mvp_ {};
  std::unique_ptr<EmitterGraphicsComponent> exhaust_emitter_ {nullptr};
};

class DemoInputComponent: public InputComponent {
 public:
  DemoInputComponent(PlayerPhysicsComponent* physics): InputComponent(physics) {}
  virtual void update(GameEntity& player, float delta_time) override;
 private:
  static constexpr int kThrustingInterval_ {5000};
  bool   thrust_ {};
  Uint32 thrusting_timer_ {SDL2_Timer::SDL2Ticks()};
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

  PlayerPhysicsComponent(GameEntity* owner, PlayerGraphicsComponent* graphics) noexcept;
  PlayerPhysicsComponent(const PlayerPhysicsComponent& other) = delete;
  PlayerPhysicsComponent(PlayerPhysicsComponent&& other) { *this = std::move(other); }

  PlayerPhysicsComponent& operator=(const PlayerPhysicsComponent& other) = delete;
  PlayerPhysicsComponent& operator=(PlayerPhysicsComponent&& other) noexcept;

  virtual void collide(const GameEntity* other) override {}
  virtual void update(const GameEntity& player, float delta_time) override;

 private:

  void checkWrap();
  void setBox2D();
  void updateMVP();

  PlayerGraphicsComponent* graphics_ {nullptr};
  bool thrusting_ {false};
  float cos_ {};
  float sin_ {};
  std::unique_ptr<EmitterPhysicsComponent> exhaust_emitter_ {nullptr};
};

} // namespace ktp
