#ifndef AEROLITS_SRC_INCLUDE_BACKGROUND_HPP_
#define AEROLITS_SRC_INCLUDE_BACKGROUND_HPP_

#include "palette.hpp"
#include <SDL.h>
#include <array>
#include <vector>

namespace ktp {

class SDL2_Renderer;

struct Star {
  SDL_Color  color_{};
  SDL_FPoint delta_{};
  SDL_FPoint position_{};
};

class Background {
 public:

  Background(const SDL_Point& screen_size);

  void draw(const SDL2_Renderer& renderer) const;
  void update(float delta_time);
  
 private:
 
  void generateStars();

  const SDL_Point screen_size_;
  const SDL_Rect  background_shape_{0, 0, screen_size_.x, screen_size_.y};
  const SDL_Color background_color_{Colors::black};
  const std::array<SDL_Color, 4> star_colors_{Colors::purple,    Colors::copper_green, 
                                              Colors::turquoise, Colors::yellow};
  
  // float angle_ = 0.f;
  std::vector<Star> stars_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_INCLUDE_BACKGROUND_HPP_