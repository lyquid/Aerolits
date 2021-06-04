#ifndef AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_

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

  static KioskState kiosk_;
  static MenuState menu_;
  static PausedState paused_;
  static PlayingState playing_;
};

class KioskState : public GameState {

};

class MenuState : public GameState {

};

class PausedState : public GameState {

};

class PlayingState : public GameState {
 public:
  virtual void draw(Game& game);
  virtual void handleEvents(Game& game);
  virtual void update(Game& game, float delta_time);
 private:
  void handleSDL2KeyEvents(Game& game, SDL_Keycode key);
  SDL_Event sdl_event_ {};
  bool debug_draw_ {false};
};

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_STATE_HPP_