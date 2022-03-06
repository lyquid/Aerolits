#include "sdl2_log.hpp"
#include "sdl2_renderer.hpp"
#include <cctype>  // std::toupper

void ktp::SDL2Video::logVideoDrivers() {
  const auto num {SDL_GetNumVideoDrivers()};
  if (num >= 1) {
    for (auto i = 0; i < num; ++i) {
      logInfo(SDL_GetVideoDriver(i));
    }
  } else {
    logSDL2Error("SDL_GetNumVideoDrivers");
  }
}

/* SDL2_Renderer */

ktp::SDL2_Renderer& ktp::SDL2_Renderer::operator=(SDL2_Renderer&& other) noexcept {
  if (this != &other) {
    if (renderer_) SDL_DestroyRenderer(renderer_);
    renderer_      = std::exchange(other.renderer_, nullptr);
    renderer_info_ = std::move(other.renderer_info_);
  }
  return *this;
}

bool ktp::SDL2_Renderer::create(const SDL2_Window& window, Uint32 flags, const std::string& scale_q) {
  if (renderer_) SDL_DestroyRenderer(renderer_);
  renderer_= SDL_CreateRenderer(window.getWindow(), -1, flags);
  if (!renderer_) {
    logSDL2Error("SDL_CreateRenderer", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    getRendererInfo();
    if (scale_q == std::string{"linear"} || scale_q == std::string{"best"}) {
      SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scale_q.c_str());
    }
    SDL_RenderSetLogicalSize(renderer_, window.getSize().x, window.getSize().y);
    return true;
  }
}

bool ktp::SDL2_Renderer::create(const SDL2_Window& window, const SDL_Point& size, Uint32 flags, const std::string& scale_q) {
  if (renderer_) SDL_DestroyRenderer(renderer_);
  renderer_ = SDL_CreateRenderer(window.getWindow(), -1, flags);
  if (!renderer_) {
    logSDL2Error("SDL_CreateRenderer", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    getRendererInfo();
    if (scale_q == std::string{"linear"} || scale_q == std::string{"best"}) {
      SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scale_q.c_str());
    }
    SDL_RenderSetLogicalSize(renderer_, size.x, size.y);
    return true;
  }
}

void ktp::SDL2_Renderer::drawCircle(const b2Vec2& center, float radius) const {
  const int32_t diameter = radius * 2 ;

  int32_t x = radius - 1;
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = tx - diameter;

  while (x >= y) {
    //  Each of the following renders an octant of the circle
    SDL_RenderDrawPoint(renderer_, center.x + x, center.y - y);
    SDL_RenderDrawPoint(renderer_, center.x + x, center.y + y);
    SDL_RenderDrawPoint(renderer_, center.x - x, center.y - y);
    SDL_RenderDrawPoint(renderer_, center.x - x, center.y + y);
    SDL_RenderDrawPoint(renderer_, center.x + y, center.y - x);
    SDL_RenderDrawPoint(renderer_, center.x + y, center.y + x);
    SDL_RenderDrawPoint(renderer_, center.x - y, center.y - x);
    SDL_RenderDrawPoint(renderer_, center.x - y, center.y + x);

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}

void ktp::SDL2_Renderer::drawCircle(const SDL_Point& center, float radius) const {
  const int32_t diameter = radius * 2;

  int32_t x = radius - 1;
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = tx - diameter;

  while (x >= y) {
    //  Each of the following renders an octant of the circle
    SDL_RenderDrawPoint(renderer_, center.x + x, center.y - y);
    SDL_RenderDrawPoint(renderer_, center.x + x, center.y + y);
    SDL_RenderDrawPoint(renderer_, center.x - x, center.y - y);
    SDL_RenderDrawPoint(renderer_, center.x - x, center.y + y);
    SDL_RenderDrawPoint(renderer_, center.x + y, center.y - x);
    SDL_RenderDrawPoint(renderer_, center.x + y, center.y + x);
    SDL_RenderDrawPoint(renderer_, center.x - y, center.y - x);
    SDL_RenderDrawPoint(renderer_, center.x - y, center.y + x);

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}

void ktp::SDL2_Renderer::drawCircle(const SDL_FPoint& center, float radius) const {
  const int32_t diameter = radius * 2;

  int32_t x = radius - 1;
  int32_t y = 0;
  int32_t tx = 1;
  int32_t ty = 1;
  int32_t error = tx - diameter;

  while (x >= y) {
    //  Each of the following renders an octant of the circle
    SDL_RenderDrawPoint(renderer_, center.x + x, center.y - y);
    SDL_RenderDrawPoint(renderer_, center.x + x, center.y + y);
    SDL_RenderDrawPoint(renderer_, center.x - x, center.y - y);
    SDL_RenderDrawPoint(renderer_, center.x - x, center.y + y);
    SDL_RenderDrawPoint(renderer_, center.x + y, center.y - x);
    SDL_RenderDrawPoint(renderer_, center.x + y, center.y + x);
    SDL_RenderDrawPoint(renderer_, center.x - y, center.y - x);
    SDL_RenderDrawPoint(renderer_, center.x - y, center.y + x);

    if (error <= 0) {
      ++y;
      error += ty;
      ty += 2;
    }

    if (error > 0) {
      --x;
      tx += 2;
      error += (tx - diameter);
    }
  }
}

void ktp::SDL2_Renderer::drawCross() const {
  int w, h;
  SDL_GetRendererOutputSize(renderer_, &w, &h);
  SDL_RenderDrawLineF(renderer_, w * 0.5f, 0, w * 0.5f, h);
  SDL_RenderDrawLineF(renderer_, 0, h * 0.5f, w, h * 0.5f);
}

void ktp::SDL2_Renderer::drawGrid(int size) const {
  int w, h;
  SDL_GetRendererOutputSize(renderer_, &w, &h);
  for (auto y = size; y < h; y += size) {
    SDL_RenderDrawLine(renderer_, 0, y, w, y);
  }
  for (auto x = size; x < w; x += size) {
    SDL_RenderDrawLine(renderer_, x, 0, x, h);
  }
}

bool ktp::SDL2_Renderer::getRendererInfo() {
  if (SDL_GetRendererInfo(renderer_, &renderer_info_) != 0) {
    logSDL2Error("SDL_GetRendererInfo", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    std::string texture_formats {};
    for (auto i = 0u; i < renderer_info_.num_texture_formats; ++i) {
      switch (renderer_info_.texture_formats[i]) {
        case SDL_PIXELFORMAT_UNKNOWN: texture_formats += "UNKNOWN "; break;
        case SDL_PIXELFORMAT_INDEX1LSB: texture_formats += "INDEX1LSB "; break;
        case SDL_PIXELFORMAT_INDEX1MSB: texture_formats += "INDEX1MSB "; break;
        case SDL_PIXELFORMAT_INDEX4LSB: texture_formats += "INDEX4LSB "; break;
        case SDL_PIXELFORMAT_INDEX4MSB: texture_formats += "INDEX4MSB "; break;
        case SDL_PIXELFORMAT_INDEX8: texture_formats += "INDEX8 "; break;
        case SDL_PIXELFORMAT_RGB332: texture_formats += "RGB332 "; break;
        case SDL_PIXELFORMAT_RGB444: texture_formats += "RGB444 "; break;
        case SDL_PIXELFORMAT_RGB555: texture_formats += "RGB555 "; break;
        case SDL_PIXELFORMAT_BGR555: texture_formats += "BGR555 "; break;
        case SDL_PIXELFORMAT_ARGB4444: texture_formats += "ARGB4444 "; break;
        case SDL_PIXELFORMAT_RGBA4444: texture_formats += "RGBA4444 "; break;
        case SDL_PIXELFORMAT_ABGR4444: texture_formats += "ABGR4444 "; break;
        case SDL_PIXELFORMAT_BGRA4444: texture_formats += "BGRA4444 "; break;
        case SDL_PIXELFORMAT_ARGB1555: texture_formats += "ARGB1555 "; break;
        case SDL_PIXELFORMAT_RGBA5551: texture_formats += "RGBA5551 "; break;
        case SDL_PIXELFORMAT_ABGR1555: texture_formats += "ABGR1555 "; break;
        case SDL_PIXELFORMAT_BGRA5551: texture_formats += "BGRA5551 "; break;
        case SDL_PIXELFORMAT_RGB565: texture_formats += "RGB565 "; break;
        case SDL_PIXELFORMAT_BGR565: texture_formats += "BGR565 "; break;
        case SDL_PIXELFORMAT_RGB24: texture_formats += "RGB24 "; break;
        case SDL_PIXELFORMAT_BGR24: texture_formats += "BGR24 "; break;
        case SDL_PIXELFORMAT_RGB888: texture_formats += "RGB888 "; break;
        case SDL_PIXELFORMAT_RGBX8888: texture_formats += "RGBX8888 "; break;
        case SDL_PIXELFORMAT_BGR888: texture_formats += "BGR888 "; break;
        case SDL_PIXELFORMAT_BGRX8888: texture_formats += "BGRX8888 "; break;
        case SDL_PIXELFORMAT_ARGB8888: texture_formats += "ARGB8888 "; break;
        case SDL_PIXELFORMAT_RGBA8888: texture_formats += "RGBA8888 "; break;
        case SDL_PIXELFORMAT_ABGR8888: texture_formats += "ABGR8888 "; break;
        case SDL_PIXELFORMAT_BGRA8888: texture_formats += "BGRA8888 "; break;
        case SDL_PIXELFORMAT_ARGB2101010: texture_formats += "ARGB2101010 "; break;
        // case SDL_PIXELFORMAT_RGBA32:
        // case SDL_PIXELFORMAT_ARGB32:
        // case SDL_PIXELFORMAT_BGRA32:
        // case SDL_PIXELFORMAT_ABGR32:
        case SDL_PIXELFORMAT_YV12: texture_formats += "YV12 "; break;
        case SDL_PIXELFORMAT_IYUV: texture_formats += "IYUV "; break;
        case SDL_PIXELFORMAT_YUY2: texture_formats += "YUY2 "; break;
        case SDL_PIXELFORMAT_UYVY: texture_formats += "UYVY "; break;
        case SDL_PIXELFORMAT_YVYU: texture_formats += "YVYU "; break;
        case SDL_PIXELFORMAT_NV12: texture_formats += "NV12 "; break;
        case SDL_PIXELFORMAT_NV21: texture_formats += "NV21 "; break;
        default: texture_formats += "UNKNOWN "; break;
      }
    }

    const auto flags {renderer_info_.flags};
    std::string flags_in_text {};
    if ((flags & SDL_RENDERER_SOFTWARE) == SDL_RENDERER_SOFTWARE) {
      flags_in_text += "SDL_RENDERER_SOFTWARE | ";
    }
    if ((flags & SDL_RENDERER_ACCELERATED) == SDL_RENDERER_ACCELERATED) {
      flags_in_text += "SDL_RENDERER_ACCELERATED | ";
    }
    if ((flags & SDL_RENDERER_PRESENTVSYNC) == SDL_RENDERER_PRESENTVSYNC) {
      flags_in_text += "SDL_RENDERER_PRESENTVSYNC | ";
    }
    if ((flags & SDL_RENDERER_TARGETTEXTURE) == SDL_RENDERER_TARGETTEXTURE) {
      flags_in_text += "SDL_RENDERER_TARGETTEXTURE";
    }

    std::string renderer_name {renderer_info_.name};
    for (auto& letter: renderer_name) letter = std::toupper(letter);

    logInfo("SDL_Renderer is " + renderer_name, SDL_LOG_CATEGORY_RENDER);
    logInfo(std::to_string(renderer_info_.num_texture_formats) + " available texture format(s): " + texture_formats, SDL_LOG_CATEGORY_RENDER);
    logInfo("Max texture size " + std::to_string(renderer_info_.max_texture_width) + 'x' + std::to_string(renderer_info_.max_texture_height), SDL_LOG_CATEGORY_RENDER);

    return true;
  }
}
