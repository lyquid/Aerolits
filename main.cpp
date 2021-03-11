#include "src/game.h"

int main(int argv, char* args[]) {
  
  using namespace ktp;

  loadConfigFiles();

  Game game{};

  if (game.init()) {

    while (!game.quit()) {
      game.handleSDL2Events();
      game.update();
      game.draw();
    }
    
    return 0;

  } else {
    // something went wrong
    return 1;
  }
}
