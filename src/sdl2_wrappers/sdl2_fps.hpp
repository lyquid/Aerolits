#ifndef KTP_SDL2_WRAPPERS_SDL2_FPS_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_FPS_HPP_

#include "sdl2_timer.hpp"

namespace ktp {

/* Just ++FPS every SDL_RenderPresent and
*  get your average fps calling "average()" */
class SDL2_FPS {
 public:

  SDL2_FPS& operator++() {
    ++counted_frames_;
    return *this;
  }
  //SDL2_FPS operator++(int) { operator++(); }

  inline void addFrame(unsigned int inc = 1u) { counted_frames_ += inc; }

  inline auto average() const { return counted_frames_ / (fps_timer_.milliseconds() / 1000.f); }

  inline void start() { fps_timer_.start(); }

 private:

  unsigned long long counted_frames_{0};
  SDL2_Timer fps_timer_{};
};

} // namespace ktp
#endif // KTP_SDL2_WRAPPERS_SDL2_FPS_HPP_