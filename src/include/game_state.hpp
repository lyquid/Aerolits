#ifndef AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_

#include "../sdl2_wrappers/sdl2_timer.hpp"

// https://gameprogrammingpatterns.com/state.html

namespace ktp {

class Game;
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

  inline static GameState* goToState(Game& game, GameState& state) { return state.enter(game); }

  static DemoState    demo_;
  static PausedState  paused_;
  static PlayingState playing_;
  static TestingState testing_;
  static TitleState   title_;

 protected:

  inline virtual GameState* enter(Game& game) { return this; }
  virtual void handleSDL2KeyEvents(Game&, SDL_Keycode) = 0;
  void setWindowTitle(Game& game);
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

class TestingState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  virtual GameState* enter(Game& game) override;
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key) override;
  bool wireframe_mode_ {false};
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
