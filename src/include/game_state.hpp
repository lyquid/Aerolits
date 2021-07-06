#ifndef AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_

#include "../../sdl2_wrappers/sdl2_timer.hpp"

// https://gameprogrammingpatterns.com/state.html

namespace ktp {

class Game;
class DemoState;
class PausedState;
class PlayingState;
class TitleState;

class GameState {
 public:
  virtual ~GameState() {}
  virtual void draw(Game&) = 0;
  virtual void handleEvents(Game&) = 0;
  virtual void update(Game&, float) = 0;
  inline virtual GameState* enter(Game& game) { return this; }

  inline static GameState* goToState(Game& game, GameState& state) { return state.enter(game); }

  static DemoState    demo_;
  static PausedState  paused_;
  static PlayingState playing_;
  static TitleState   title_;
 protected:
  void setWindowTitle(Game& game);
};

class DemoState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
  virtual GameState* enter(Game& game) override;
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
  Uint32 blink_timer_ {};
  bool blink_flag_ {true};
};

class PausedState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
  virtual GameState* enter(Game& game) override;
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
  Uint32 blink_timer_ {};
  bool blink_flag_ {true};
};

class PlayingState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
  virtual GameState* enter(Game& game) override;
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
};

class TitleState: public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
  virtual GameState* enter(Game& game) override;
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
  static inline constexpr float kDefaultBackgroundDeltaInMenu_ {500.f};
  static inline constexpr Uint32 kWaitForDemo_ {2000};
  Uint32 demo_time_ {};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_