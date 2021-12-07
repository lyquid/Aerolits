#include "sdl2_window.hpp"
#include "sdl2_log.hpp"

bool ktp::SDL2_Window::create(const std::string& title, const SDL_Point& size, Uint32 flags) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  window_.reset(SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, size.x, size.y, flags));
  if (window_ == nullptr) {
	  logSDL2Error("SDL_CreateWindow", SDL_LOG_CATEGORY_SYSTEM);
    return false;
	}
  SDL_GetWindowSize(window_.get(), &size_.x, &size_.y);
  return true;
}
