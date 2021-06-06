#ifndef AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_

#include "../../sdl2_wrappers/sdl2_timer.hpp"

// https://gameprogrammingpatterns.com/state.html

namespace ktp {

class Game;
class KioskState;
class MenuState;
class PausedState;
class PlayingState;

class GameState {
 public:
  virtual ~GameState() {}
  virtual void draw(Game&) {}
  virtual void handleEvents(Game&) {}
  virtual void update(Game&, float) {}
  inline virtual GameState* enter() { return this; }

  static KioskState kiosk_;
  static MenuState menu_;
  static PausedState paused_;
  static PlayingState playing_;
 public:
  inline static GameState* goToState(GameState& state) { return state.enter(); }
};

class KioskState : public GameState {};

class MenuState : public GameState {};

class PausedState : public GameState {
 public:
  virtual void draw(Game& game);
  virtual void handleEvents(Game& game);
  virtual void update(Game& game, float delta_time);
  virtual GameState* enter();
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
  Uint32 blink_timer_ {};
  bool blink_flag_ {true};
};

class PlayingState : public GameState {
 public:
  virtual void draw(Game& game);
  virtual void handleEvents(Game& game);
  virtual void update(Game& game, float delta_time);
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_