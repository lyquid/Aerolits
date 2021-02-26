#ifndef KTP_SDL2_WRAPPERS_SDL2_TIMER_H_
#define KTP_SDL2_WRAPPERS_SDL2_TIMER_H_

#include <SDL.h>

namespace ktp {

class SDL2_Timer {
 public:
  SDL2_Timer();

  bool isPaused() const { return started_ && paused_; }
  bool isStarted() const { return started_; }
  Uint32 getTicks() const;
  void pause();
  void resume();
  float restart();
  void start();
  void stop();

 private: 
  Uint32 paused_ticks_;
  Uint32 start_ticks_;
  bool paused_;
  bool started_;
};

} //end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_TIMER_H_