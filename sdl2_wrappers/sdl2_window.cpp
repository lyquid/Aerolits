#include "sdl2_window.h"

bool ktp::SDL2_Window::create(const SDL_Point& size) {
  window_.reset(SDL_CreateWindow("KUGE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, SDL_WINDOW_SHOWN));
  if (window_ == nullptr) {
	  ktp::logSDL2Error("SDL_CreateWindow");
    return false;
	}
  SDL_GetWindowSize(window_.get(), &size_.x, &size_.y);
  return true;
}
