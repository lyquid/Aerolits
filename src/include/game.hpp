#ifndef AEROLITS_SRC_INCLUDE_GAME_HPP_
#define AEROLITS_SRC_INCLUDE_GAME_HPP_

#include "background.hpp"
#include "debug_draw.hpp"
#include "game_entity.hpp"
#include "game_state.hpp"
#include "../../kuge/kuge.hpp"
#include "../../sdl2_wrappers/sdl2_wrappers.hpp"
#include <box2d/box2d.h>
#include <memory>
#include <string>
#include <vector>

namespace ktp {

class Emitter;

void loadConfigFiles();

class Game {

 public:

  Game();
  ~Game() { clean(); }
  Game(const Game& other) = delete;
  Game(Game&& other) = delete;

  Game& operator=(const Game& other) = delete;
  Game& operator=(Game&& other) = delete;

  inline void draw() { state_->draw(*this); }
  inline void handleEvents() { state_->handleEvents(*this); }
  bool init();
  inline void setFrameTime(double time) { frame_time_ = time; }
  inline bool quit() const { return quit_; }
  inline void reset() { aerolites_.clear(); projectiles_.clear(); }
  inline void update(float delta_time) { state_->update(*this, delta_time); }

 private:

  friend class GameState;
  friend class DemoState;
  friend class PausedState;
  friend class PlayingState;
  friend class TitleState;
  friend class InputComponent;

  void clean();
  bool initSDL2();
  bool loadResources();

  const std::string kGameTitle_ {"Aerolits"};
  SDL_Point screen_size_ {1366, 768};
  bool paused_ {false};
  bool quit_ {false};
  SDL2_Window main_window_ {};
  SDL2_Renderer renderer_ {};
  SDL2_Font font_ {};
  /* State */
  GameState* state_ {nullptr};
  /* FPS */
  double frame_time_ {};
  /* KUGE engine */
  kuge::EventBus event_bus_ {};
  kuge::AudioSystem audio_sys_ {event_bus_};
  kuge::InputSystem input_sys_ {event_bus_};
  kuge::OutputSystem output_sys_ {event_bus_};
  /* Box2D */
  b2World world_ {b2Vec2{0.f, 0.f}};
  int32 velocity_iterations_ {8};
  int32 position_iterations_ {3};
  DebugDraw debug_draw_ {};
  bool debug_draw_on_ {false};
  /* Player */
  std::unique_ptr<GameEntity> player_ {};
  /* Background */
  std::unique_ptr<GameEntity> background_ {};
  /* Emitters */
  static std::vector<GameEntity> emitters_;
  /* Aerolites */
  static std::vector<GameEntity> aerolites_;
  /* Projectiles */
  static std::vector<GameEntity> projectiles_;
  /* Game texts */
  SDL2_Texture demo_text_ {};
  SDL2_Texture paused_text_ {};
  SDL2_Texture title_text_ {};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_GAME_HPP_