#include "sdl2_timer.hpp"

Uint32 ktp::SDL2_Timer::getTicks() const {
  if (started_) {
    return paused_ ? paused_ticks_ : SDL_GetTicks() - start_ticks_;
  }
  return 0u;
}

void ktp::SDL2_Timer::pause() {
  if (started_ && !paused_) {
    paused_ = true;
    paused_ticks_ = SDL_GetTicks() - start_ticks_;
    start_ticks_ = 0u;
  }
}

void ktp::SDL2_Timer::resume() {
  if (started_ && paused_) {
    paused_ = false;
    start_ticks_ = SDL_GetTicks() - paused_ticks_;
    paused_ticks_ = 0u;
  }
}

Uint32 ktp::SDL2_Timer::restart() {
  const auto time{getTicks()};
  stop();
  start();
  return time;
}

void ktp::SDL2_Timer::start() {
  started_ = true;
  paused_ = false;
  start_ticks_ = SDL_GetTicks();
  paused_ticks_ = 0u;
}

void ktp::SDL2_Timer::stop() {
  started_ = false;
  paused_ = false;
  start_ticks_ = 0u;
  paused_ticks_ = 0u;
}