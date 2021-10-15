#ifndef KTP_SDL2_WRAPPERS_SDL2_TIMER_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_TIMER_HPP_

#include <SDL.h>

namespace ktp {

/**
 * @brief An SDL2 based timer for, you know, keep track of time passed.
 *        Resolution is milliseconds (1000th part of a second).
 */
class SDL2_Timer {
 public:

  /**
   * @return The number of milliseconds since the SDL library initialization.
   */
  static inline auto SDL2Ticks() { return SDL_GetTicks(); }

  /**
   * @return The number of milliseconds since the timer started.
   */
  auto milliseconds() const {
    if (started_) return paused_ ? paused_ticks_ : SDL_GetTicks() - start_ticks_;
    return (Uint32)0;
  }

  /**
   * @brief Pauses the timer.
   */
  void pause() {
    if (started_ && !paused_) {
      paused_ = true;
      paused_ticks_ = SDL_GetTicks() - start_ticks_;
      start_ticks_ = 0u;
    }
  }

  /**
   * @return True if the timer is currently paused. False otherwise.
   */
  inline auto paused() const { return started_ && paused_; }

  /**
   * @brief Restarts the timer setting the time passed to 0.
   * @return The number of milliseconds since the timer started.
   */
  auto restart() {
    const auto time {milliseconds()};
    stop();
    start();
    return time;
  }

  /**
   * @brief Resumes time counting.
   */
  void resume() {
    if (started_ && paused_) {
      paused_ = false;
      start_ticks_ = SDL_GetTicks() - paused_ticks_;
      paused_ticks_ = 0u;
    }
  }

  /**
   * @return The number of seconds since the timer started.
   */
  float seconds() const {
    if (started_) return paused_ ? paused_ticks_ * 0.001f : (SDL_GetTicks() - start_ticks_) * 0.001f;
    return 0;
  }

  /**
   * @brief Begins to count the time.
   */
  void start() {
    started_ = true;
    paused_ = false;
    start_ticks_ = SDL_GetTicks();
    paused_ticks_ = 0u;
  }

  /**
   * @return True if the timer is currently started. False otherwise.
   */
  inline auto started() const { return started_; }

  /**
   * @brief Stops the timer, reseting all flags and counter to default state.
   */
  void stop() {
    started_ = false;
    paused_ = false;
    start_ticks_ = 0u;
    paused_ticks_ = 0u;
  }

 private:

  Uint32 paused_ticks_ {0};
  Uint32 start_ticks_ {0};
  bool paused_ {false};
  bool started_ {false};
};

} //end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_TIMER_HPP_