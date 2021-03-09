#ifndef AEROLITS_SRC_GAME_H_
#define AEROLITS_SRC_GAME_H_

#include "background.h"
#include "palette.h"
#include "player.h"
#include "../include/resources_path.h"
#include "../kuge/kuge.h"
#include "../sdl2_wrappers/sdl2_wrappers.h"

#include <sstream>

namespace ktp {

class Game {
 public:

  Game();
  ~Game() { clean(); }
  void draw();
  void handleSDL2Events();
  void handleSDL2KeyEvents(const SDL_Keycode& key);
  bool init();
  bool quit() const { return quit_; }
  void update();

 private:
 
  void checkKeyStates(float delta_time);
  void clean();
  bool initSDL2();
  bool loadResources();

  ktp::SDL2_Timer clock_{};
  SDL_Point screen_size_{1200, 900};
  bool quit_ = false;

  ktp::SDL2_Window main_window_{};
  ktp::SDL2_Renderer renderer_{};
  SDL_Event sdl_event_{};
  ktp::SDL2_Font font_{};
  /* FPS */
  ktp::SDL2_FPS     fps_{};
  std::stringstream fps_text_{};
  ktp::SDL2_Texture fps_texture_{};
  /* KUGE engine */
  kuge::EventBus event_bus_{};
  kuge::AudioSystem audio_sys_{event_bus_};
  kuge::InputSystem input_sys_{event_bus_};
  kuge::OutputSystem output_sys_{event_bus_};
  /* Player */
  ktp::Player player_{screen_size_, event_bus_};
  /* Background */
  ktp::Background background_{screen_size_};
};

} // end namespace ktp

#endif // AEROLITS_SRC_GAME_H_