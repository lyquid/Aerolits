#ifndef AEROLITS_SRC_BACKGROUND_H_
#define AEROLITS_SRC_BACKGROUND_H_

#include "palette.h"
#include "../kuge/kuge.h"
#include "../sdl2_wrappers/sdl2_wrappers.h"
#include <array>
#include <random>
#include <vector>

namespace ktp {

struct Star {
  SDL_Color  color_{};
  SDL_FPoint delta_{};
  SDL_FPoint position_{};
};

class Background {
 public:

  Background(const SDL_Point& screen_size);
  void draw(SDL2_Renderer& renderer) const;
  void update(float delta_time);
  
 private:
 
  void generateStars();

  const SDL_Point screen_size_;
  const SDL_Rect  background_shape_{0, 0, screen_size_.x, screen_size_.y};
  const SDL_Color background_color_{ktp::Colors::black};
  const std::array<SDL_Color, 4> star_colors_{ktp::Colors::purple,    ktp::Colors::copper_green, 
                                              ktp::Colors::turquoise, ktp::Colors::yellow};
  
  // float angle_ = 0.f;
  std::vector<Star> stars_{};
};

} // end namespace ktp

#endif // AEROLITS_SRC_BACKGROUND_H_