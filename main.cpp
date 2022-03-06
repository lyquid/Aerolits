#include "src/include/config_parser.hpp"
#include "src/include/game.hpp"
#include <ctime>

int main(int argv, char* args[]) {

  using namespace ktp;

  ConfigParser::loadConfigFiles();

  srand(time(nullptr));

  Game game {};

  constexpr double dt = 0.01;
  double current_time = SDL_GetTicks() / 1000.0;
  double accumulator = 0.0;

  while (!game.quit()) {

    double new_time = SDL_GetTicks() / 1000.0;
    double frame_time = new_time - current_time;
    game.setFrameTime(frame_time);
    current_time = new_time;
    accumulator += frame_time;

    game.handleEvents();

    while (accumulator >= dt) {
      game.update(dt);
      accumulator -= dt;
    }

    game.draw();
  }
  return 0;
}
