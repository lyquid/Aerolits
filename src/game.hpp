#ifndef AEROLITS_SRC_GAME_H_
#define AEROLITS_SRC_GAME_H_

#include "background.hpp"
#include "palette.hpp"
#include "particle_system.hpp"
#include "player.hpp"
#include "../include/resources_path.hpp"
#include "../kuge/kuge.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"

#include <list>
#include <string>

namespace ktp {

void loadConfigFiles();

class Game {
 public:

  Game();
  ~Game() { clean(); }
  void draw();
  void handleSDL2Events();
  void handleSDL2KeyEvents(const SDL_Keycode& key);
  bool init();
  bool quit() const { return quit_; }
  void update(float delta_time);

 private:
 
  void checkKeyStates(float delta_time);
  void clean();
  bool initSDL2();
  bool loadResources();

  const std::string kGameTitle_{"Aeròlits"};
  SDL_Point screen_size_{1366, 768};
  bool quit_{false};

  ktp::SDL2_Window main_window_{};
  ktp::SDL2_Renderer renderer_{};
  SDL_Event sdl_event_{};
  ktp::SDL2_Font font_{};
  /* FPS */
  ktp::SDL2_FPS fps_{};
  /* KUGE engine */
  kuge::EventBus event_bus_{};
  kuge::AudioSystem audio_sys_{event_bus_};
  kuge::InputSystem input_sys_{event_bus_};
  kuge::OutputSystem output_sys_{event_bus_};
  /* Player */
  ktp::Player player_{screen_size_, event_bus_};
  /* Background */
  ktp::Background background_{screen_size_};
  /* Emitters */
  std::list<Emitter> emitters_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_GAME_H_