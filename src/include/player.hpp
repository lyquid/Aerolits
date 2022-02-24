#pragma once

#include "config_parser.hpp"
#include "graphics_component.hpp"
#include "physics_component.hpp"
#include "resources.hpp"
#include "../sdl2_wrappers/sdl2_opengl.hpp"
#include "../sdl2_wrappers/sdl2_timer.hpp"
#include <utility> // std::move
#include <memory>

namespace ktp {

class EmitterPhysicsComponent;

class PlayerGraphicsComponent: public GraphicsComponent {

  friend class PlayerPhysicsComponent;

 public:

  PlayerGraphicsComponent();
  virtual void update(const GameEntity& player) override;

 private:

  void generateOpenGLStuff(float size);

  Color color_ {ConfigParser::player_config.color_};
  VAO vao_ {};
  VBO vertices_ {};
  EBO vertices_indices_ {};
  ShaderProgram shader_ {Resources::getShader("player")};
  glm::mat4 mvp_ {};
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

  PlayerPhysicsComponent(GameEntity* owner, PlayerGraphicsComponent* graphics);
  PlayerPhysicsComponent(const PlayerPhysicsComponent& other) = delete;
  PlayerPhysicsComponent(PlayerPhysicsComponent&& other) { *this = std::move(other); }
  ~PlayerPhysicsComponent();

  PlayerPhysicsComponent& operator=(const PlayerPhysicsComponent& other) = delete;
  PlayerPhysicsComponent& operator=(PlayerPhysicsComponent&& other);

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
  EmitterPhysicsComponent* exhaust_emitter_ {nullptr};
};

} // namespace ktp
