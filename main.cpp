#include "src/include/game.hpp"
#include <stdlib.h>
#include <time.h>

int main(int argv, char* args[]) {
  
  using namespace ktp;

  loadConfigFiles();

  srand(time(nullptr));

  Game game{};

  constexpr double dt = 0.01;
  double current_time = SDL_GetTicks() / 1000.0;
  double accumulator = 0.0;

  if (game.init()) {

    while (!game.quit()) {

      double new_time = SDL_GetTicks() / 1000.0;
      double frame_time = new_time - current_time;
      current_time = new_time;
      accumulator += frame_time;

      game.handleSDL2Events();

      while (accumulator >= dt) {
        game.update(dt);
        accumulator -= dt;
      }

      game.draw();
    }
    
    return 0;

  } else {
    // something went wrong
    return 1;
  }
}
