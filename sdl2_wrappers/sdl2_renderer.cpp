#include "sdl2_renderer.h"

bool ktp::SDL2_Renderer::clear() const {
  if (SDL_RenderClear(renderer_.get()) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::create(const SDL2_Window& window) {
  renderer_.reset(SDL_CreateRenderer(window.getWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
  if (renderer_ == nullptr) {
    ktp::logSDL2Error("SDL_CreateRenderer");
    return false;
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(renderer_.get(), window.getSize().x, window.getSize().y);
  return true;
}

bool ktp::SDL2_Renderer::drawLines(const std::vector<SDL_Point>& points) const {
  if (SDL_RenderDrawLines(renderer_.get(), points.data(), points.size()) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::drawLines(const std::vector<SDL_FPoint>& points) const {
  if (SDL_RenderDrawLinesF(renderer_.get(), points.data(), points.size()) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::drawPoint(const SDL_Point& point) const {
  if (SDL_RenderDrawPoint(renderer_.get(), point.x, point.y) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::drawPoint(const SDL_FPoint& point) const {
  if (SDL_RenderDrawPointF(renderer_.get(), point.x, point.y) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::drawPoints(const std::vector<SDL_Point>& points) const {
  if (SDL_RenderDrawPoints(renderer_.get(), points.data(), points.size()) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::drawPoints(const std::vector<SDL_FPoint>& points) const {
  if (SDL_RenderDrawPointsF(renderer_.get(), points.data(), points.size()) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::drawRect(const SDL_Rect& rect) const {
  if (SDL_RenderDrawRect(renderer_.get(), &rect)) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::drawRectFill(const SDL_Rect& rect) const {
  if (SDL_RenderFillRect(renderer_.get(), &rect) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::setDrawColor(const SDL_Color& color) const {
  if (SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a) == 0) {
    return true;
  }
  return false;
}

bool ktp::SDL2_Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
  if (SDL_SetRenderDrawColor(renderer_.get(), r, g, b, a) == 0) {
    return true;
  }
  return false;
}