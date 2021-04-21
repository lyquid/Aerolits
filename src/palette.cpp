#include "palette.hpp"
#include "../sdl2_wrappers/sdl2_wrappers.hpp"
#include <cmath> // sqrt
#include <string>

auto ktp::Colors::colorDistance (const SDL_Color& color1, const SDL_Color& color2) {
  const auto rmean {(static_cast<long>(color1.r) + static_cast<long>(color2.r)) / 2};
  const auto r {static_cast<long>(color1.r) - static_cast<long>(color2.r)};
  const auto g {static_cast<long>(color1.g) - static_cast<long>(color2.g)};
  const auto b {static_cast<long>(color1.b) - static_cast<long>(color2.b)};

  return sqrt((((512 + rmean) * r * r) >> 8) + 4 * g * g + (((767 - rmean) * b * b) >> 8));
}

SDL_Color ktp::Colors::getNearestColor(const SDL_Color& color) {
  int position{};
  double lowest_distance{255 * 3};
  double current_distance{};
  
  for (auto i = 0u; i < palette.size(); ++i) {
    current_distance = colorDistance(color, palette[i]);
    if (current_distance < lowest_distance) {
      lowest_distance = current_distance;
      position = i;
    }
  }

  /* logMessage("The nearest color to {" 
    + std::to_string(color.r) + ", " 
    + std::to_string(color.g) + ", " 
    + std::to_string(color.b) + "} is {" 
    + std::to_string(palette[position].r) + ", " 
    + std::to_string(palette[position].g) + ", " 
    + std::to_string(palette[position].b) 
    + "} at position " + std::to_string(position + 1) + "."); */

  return palette[position];
}
