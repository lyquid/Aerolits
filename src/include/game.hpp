#ifndef AEROLITS_SRC_INCLUDE_GAME_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_HPP_

#include "camera.hpp"
#include "config_parser.hpp"
#include "contact_listener.hpp"
#include "game_state.hpp"
#include "../kuge/kuge.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include <box2d/box2d.h>

namespace ktp {

class Game {

  friend class DemoState;
  friend class GameState;
  friend class GameOverState;
  friend class PausedState;
  friend class PlayingState;
  friend class TestingState;
  friend class TitleState;

 public:

  Game();
  ~Game() { clean(); }
  Game(const Game& other) = delete;
  Game(Game&& other) = delete;

  Game& operator=(const Game& other) = delete;
  Game& operator=(Game&& other) = delete;

  void draw() { state_->draw(*this); }
  void handleEvents() { state_->handleEvents(*this); }
  bool quit() const { return quit_; }
  void reset();
  void update(float delta_time) { state_->update(*this, delta_time); }

  static Camera camera_;

  // FPS
  static double frame_time_;

  /**
   * @brief This timer only goes when playing or in demo state.
   */
  static SDL2_Timer gameplay_timer_;

  /**
   * @brief
   */
  static b2World b2_world_;

 private:

  void clean();
  bool initImgui();
  bool initSDL2();
  bool loadResources();

  SDL_Point screen_size_ {ConfigParser::game_config.screen_size_};
  bool paused_ {false};
  bool quit_ {false};
  SDL2_Window main_window_ {};
  SDL2_GLContext context_ {};
  // State
  GameState* state_ {nullptr};
  // KUGE engine
  kuge::EventBus event_bus_ {};
  kuge::AudioSystem audio_sys_ {};
  kuge::BackendSystem backend_sys_ {};
  kuge::GUISystem gui_sys_ {};
  kuge::InputSystem input_sys_ {};
  kuge::OutputSystem output_sys_ {ConfigParser::game_config.output_};
  // Box2D
  int32 velocity_iterations_ {8};
  int32 position_iterations_ {3};
  ContactListener contact_listener_ {};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_HPP_
