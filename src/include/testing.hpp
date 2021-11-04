#pragma once

#include "random.hpp"
#include "../sdl2_wrappers/sdl2_renderer.hpp"
#include <SDL.h>
#include <vector>

namespace ktp {

class Testing {
 public:

  void draw(const SDL2_Renderer& ren) const;
  inline void generateShape(float max_size) { generateShape(max_size, generateRand(20, 30)); }
  void generateShape(float max_size, int sides);

 private:

  std::vector<SDL_Point> polygon_ {};
  static constexpr auto kPI {3.14159265359};
  const SDL_Point center_ {600, 400};
};

} // namespace ktp