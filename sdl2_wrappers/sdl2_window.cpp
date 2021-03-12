#include "sdl2_window.hpp"

bool ktp::SDL2_Window::create(const std::string& title, const SDL_Point& size) {
  window_.reset(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, SDL_WINDOW_SHOWN));
  if (window_ == nullptr) {
	  logSDL2Error("SDL_CreateWindow");
    return false;
	}
  SDL_GetWindowSize(window_.get(), &size_.x, &size_.y);
  return true;
}
