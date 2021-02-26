#include "sdl2_timer.h"

ktp::SDL2_Timer::SDL2_Timer():
  paused_ticks_(0u),
  start_ticks_(0u),
  paused_(false),
  started_(false) {}

Uint32 ktp::SDL2_Timer::getTicks() const {
  Uint32 time = 0;
  if (started_) {
    if (paused_) {
      time = paused_ticks_;
    } else {
      time = SDL_GetTicks() - start_ticks_;
    }
  }
  return time;
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

float ktp::SDL2_Timer::restart() {
  const auto time = getTicks();
  stop();
  start();
  return static_cast<float>(time);
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
