#ifndef AEROLITS_SRC_INCLUDE_GAME_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_HPP_

#include "background.hpp"
#include "camera.hpp"
#include "config_parser.hpp"
#include "contact_listener.hpp"
#include "debug_draw.hpp"
#include "game_entity.hpp"
#include "game_state.hpp"
#include "testing.hpp"
#include "../kuge/kuge.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include <box2d/box2d.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <string>

namespace ktp {

class Game {

  friend class DemoState;
  friend class GameState;
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

  inline void draw() { state_->draw(*this); }
  inline void handleEvents() { state_->handleEvents(*this); }
  inline void setFrameTime(double time) { frame_time_ = time; }
  inline bool quit() const { return quit_; }
  void reset();
  inline void update(float delta_time) { state_->update(*this, delta_time); }

  static Camera camera_;

  /**
   * @brief This timer only goes when playing or in demo state.
   */
  static SDL2_Timer gameplay_timer_;

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
  // FPS
  double frame_time_ {};
  // KUGE engine
  kuge::EventBus event_bus_ {};
  kuge::AudioSystem audio_sys_ {};
  kuge::BackendSystem backend_sys_ {};
  kuge::GUISystem gui_sys_ {};
  kuge::InputSystem input_sys_ {};
  kuge::OutputSystem output_sys_ {ConfigParser::game_config.output_};
  // Box2D
  b2World world_ {b2Vec2{0.f, 0.f}};
  int32 velocity_iterations_ {8};
  int32 position_iterations_ {3};
  DebugDraw debug_draw_ {};
  ContactListener contact_listener_ {};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_HPP_
