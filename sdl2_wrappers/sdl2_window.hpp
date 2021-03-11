#ifndef KTP_SDL2_WRAPPERS_SDL2_WINDOW_H_
#define KTP_SDL2_WRAPPERS_SDL2_WINDOW_H_

#include "sdl2_log.hpp"
#include <SDL.h>
#include <memory>

namespace ktp {

template <typename T> struct deleter;

template <> struct deleter<SDL_Window> {
  void operator()(SDL_Window* window) { 
    SDL_DestroyWindow(window);
    window = nullptr;
  }
};

class SDL2_Window {
 public:
 
  /**
  * Creates a SDL_Window with the specified size.
  * @param size An SDL_Point structure with the desired size of the window.
  * @return True on success, or false on errors.
  */
  bool create(const SDL_Point& size);

  const SDL_Point& getSize() const { return size_; }
  SDL_Window* getWindow() const { return window_.get(); }

 private:

  template <typename T>
  using unique_ptr_deleter = std::unique_ptr<T, deleter<T>>;

  unique_ptr_deleter<SDL_Window> window_{nullptr};
  SDL_Point size_{};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_WINDOW_H_