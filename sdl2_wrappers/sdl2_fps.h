#ifndef KTP_SDL2_WRAPPERS_SDL2_FPS_H_
#define KTP_SDL2_WRAPPERS_SDL2_FPS_H_

#include "sdl2_timer.h"

namespace ktp {

/* Just ++FPS every SDL_RenderPresent and 
*  get your average fps calling "average()" */ 
class SDL2_FPS {
 public:

  SDL2_FPS& operator++() {
    ++counted_frames_;
    return *this;
  }
  SDL2_FPS operator++(int) { operator++(); }

  void addFrame(unsigned int inc = 1u) { counted_frames_ += inc; }

  auto average() const { return counted_frames_ / (fps_timer_.getTicks() / 1000.f); }

  void start() { fps_timer_.start(); }

 private:
 
  unsigned long long counted_frames_ = 0;
  SDL2_Timer fps_timer_{}; 
};

} // namespace ktp
#endif // KTP_SDL2_WRAPPERS_SDL2_FPS_H_