#ifndef AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_

#include "../../sdl2_wrappers/sdl2_timer.hpp"

// https://gameprogrammingpatterns.com/state.html

namespace ktp {

class Game;
class PausedState;
class PlayingState;
class TitleState;

class GameState {
 public:
  virtual ~GameState() {}
  virtual void draw(Game&) {}
  virtual void handleEvents(Game&) {}
  virtual void update(Game&, float) {}
  inline virtual GameState* enter() { return this; }

  static PausedState paused_;
  static PlayingState playing_;
  static TitleState title_;
 public:
  inline static GameState* goToState(GameState& state) { return state.enter(); }
};

class PausedState : public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
  virtual GameState* enter() override;
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
  Uint32 blink_timer_ {};
  bool blink_flag_ {true};
};

class PlayingState : public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
};

class TitleState : public GameState {
 public:
  virtual void draw(Game& game) override;
  virtual void handleEvents(Game& game) override;
  virtual void update(Game& game, float delta_time) override;
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
  static inline constexpr float kDefaultBackgroundDeltaInMenu_ {500.f};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_