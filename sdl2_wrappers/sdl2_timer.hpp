#ifndef KTP_SDL2_WRAPPERS_SDL2_TIMER_H_
#define KTP_SDL2_WRAPPERS_SDL2_TIMER_H_

#include <SDL.h>

namespace ktp {

class SDL2_Timer {
 public:

  static inline auto getSDL2Ticks() { return SDL_GetTicks(); }
  inline bool isPaused() const { return started_ && paused_; }
  inline bool isStarted() const { return started_; }
  Uint32 getTicks() const;
  void pause();
  void resume();
  Uint32 restart();
  void start();
  void stop();

 private:

  Uint32 paused_ticks_{0u};
  Uint32 start_ticks_{0u};
  bool paused_{false};
  bool started_{false};
};

} //end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_TIMER_H_