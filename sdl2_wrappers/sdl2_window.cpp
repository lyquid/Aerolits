#include "sdl2_window.hpp"
#include "sdl2_log.hpp"

bool ktp::SDL2_Window::create(const std::string& title, const SDL_Point& size, Uint32 flags) {
  window_.reset(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, flags));
  if (window_ == nullptr) {
	  logSDL2Error("SDL_CreateWindow");
    return false;
	}
  SDL_GetWindowSize(window_.get(), &size_.x, &size_.y);
  return true;
}
