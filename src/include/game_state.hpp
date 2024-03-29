#ifndef AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_

#include "opengl.hpp"

// https://gameprogrammingpatterns.com/state.html

namespace kuge {
  struct B2DebugFlags;
}

namespace ktp {

class Game;
class DebugDraw;
class DemoState;
class PausedState;
class PlayingState;
class TestingState;
class TitleState;

class GameState {

 public:

  virtual ~GameState() {}
  virtual void draw(Game&) = 0;
  virtual void handleEvents(Game&) = 0;
  virtual void update(Game&, float) = 0;

  static GameState* goToState(Game& game, GameState& state) { return state.enter(game); }

  static void setDebugDrawFlags(const kuge::B2DebugFlags& debug_flags);
  static void updateCulling() { culling_ ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE); }
  static void updateDeepTest() { deep_test_ ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST); }
  static void updatePolygonDraw() { polygon_draw_ ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

  static DebugDraw b2_debug_;
  static bool backend_draw_;
  static bool culling_;
  static bool debug_draw_;
  static bool deep_test_;
  static bool polygon_draw_;

  static DemoState    demo_;
  static PausedState  paused_;
  static PlayingState playing_;
  static TestingState testing_;
  static TitleState   title_;

 protected:

  virtual GameState* enter(Game& game) { return this; }
  virtual void handleSDL2KeyEvents(Game&, SDL_Keycode) = 0;
  SDL_Event sdl_event_ {};
};

class DemoState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  virtual GameState* enter(Game& game) override;
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key) override;
  Uint32 blink_timer_ {};
  bool blink_flag_ {true};
};

class PausedState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  virtual GameState* enter(Game& game) override;
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key) override;
  Uint32 blink_timer_ {};
  bool blink_flag_ {true};
};

class PlayingState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  virtual GameState* enter(Game& game) override;
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key) override;
};

class Testing;

class TestingState: public GameState {
 public:
  ~TestingState() { delete test_; }
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  virtual GameState* enter(Game& game) override;
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key) override;
  Testing* test_ {nullptr};
};

class TitleState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  virtual GameState* enter(Game& game) override;
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key) override;
  static constexpr auto kDefaultBackgroundDeltaInMenu_ {500.f};
  static constexpr Uint32 kWaitForDemo_ {2000};
  Uint32 demo_time_ {};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_
