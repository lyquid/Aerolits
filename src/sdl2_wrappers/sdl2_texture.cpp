#include "sdl2_texture.hpp"
#include "sdl2_log.hpp"

ktp::SDL2_Texture& ktp::SDL2_Texture::operator=(SDL2_Texture&& other) noexcept {
  if (this != &other) {
    if (texture_) SDL_DestroyTexture(texture_);
    renderer_ = std::exchange(other.renderer_, nullptr);
    texture_  = std::exchange(other.texture_, nullptr);
    access_   = other.access_;
    format_   = other.format_;
    height_   = other.height_;
    width_    = other.width_;
  }
  return *this;
}

bool ktp::SDL2_Texture::create(Uint32 format, int access, const SDL_Point& size) {
  if (texture_) SDL_DestroyTexture(texture_);
  texture_ = SDL_CreateTexture(renderer_, format, access, size.x, size.y);
  if (!texture_) {
    logSDL2Error("SDL_CreateTexture: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  } else {
    SDL_QueryTexture(texture_, &format_, &access_, &width_, &height_);
    return true;
  }
}

bool ktp::SDL2_Texture::loadFromFile(const std::string& path) {
  const auto surface {IMG_Load(path.c_str())};
  if (!surface) {
    logSDL2Error("IMG_Load: ", path, SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextBlended(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& color) {
  const auto surface {TTF_RenderText_Blended(font.getFont(), text.c_str(), color)};
  if (surface == nullptr) {
    logSDL2Error("TTF_RenderText_Blended: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextShaded(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& fg_color, const SDL_Color& bg_color) {
  const auto surface {TTF_RenderText_Shaded(font.getFont(), text.c_str(), fg_color, bg_color)};
  if (surface == nullptr) {
    logSDL2Error("TTF_RenderText_Shaded: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextSolid(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& color) {
  const auto surface {TTF_RenderText_Solid(font.getFont(), text.c_str(), color)};
  if (surface == nullptr) {
    logSDL2Error("TTF_RenderText_Solid: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::queryTexture(const SDL2_Texture& texture, Uint32* format, int* access, int* width, int* height) {
  if (SDL_QueryTexture(texture.getTexture(), format, access, width, height) != 0) {
    logSDL2Error("SDL_QueryTexture: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render() const {
  if (SDL_RenderCopy(renderer_, texture_, NULL, NULL) != 0) {
    logSDL2Error("SDL_RenderCopy: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_Point& where) const {
  const SDL_Rect dest {where.x, where.y, width_, height_};
  if (SDL_RenderCopy(renderer_, texture_, NULL, &dest) != 0) {
    logSDL2Error("SDL_RenderCopy: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_FPoint& where) const {
  const SDL_Rect dest {static_cast<int>(where.x), static_cast<int>(where.y), width_, height_};
  if (SDL_RenderCopy(renderer_, texture_, NULL, &dest) != 0) {
    logSDL2Error("SDL_RenderCopy: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_Rect& dest_rect) const {
  if (SDL_RenderCopy(renderer_, texture_, NULL, &dest_rect) != 0) {
    logSDL2Error("SDL_RenderCopy: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_Rect& src_rect, const SDL_Rect& dest_rect) const {
  if (SDL_RenderCopy(renderer_, texture_, &src_rect, &dest_rect) != 0) {
    logSDL2Error("SDL_RenderCopy: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_Rect& src_rect, const SDL_Rect& dest_rect, float angle) const {
  if (SDL_RenderCopyEx(renderer_, texture_, &src_rect, &dest_rect, angle, NULL, SDL_FLIP_NONE) != 0) {
    logSDL2Error("SDL_RenderCopyEx: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_FPoint& where, const SDL2_Renderer& ren) const {
  const SDL_Rect dest {static_cast<int>(where.x), static_cast<int>(where.y), width_, height_};
  if (SDL_RenderCopy(ren.renderer(), texture_, NULL, &dest) != 0) {
    logSDL2Error("SDL_RenderCopy: ", SDL_LOG_CATEGORY_RENDER);
    return false;
  }
  return true;
}

void ktp::SDL2_Texture::createTextureFromSurface(SDL_Surface& surface) {
  if (texture_) SDL_DestroyTexture(texture_);
  texture_ = SDL_CreateTextureFromSurface(renderer_, &surface);
  if (texture_) {
    SDL_QueryTexture(texture_, &format_, &access_, &width_, &height_);
  } else {
    logSDL2Error("SDL_CreateTextureFromSurface: ", SDL_LOG_CATEGORY_RENDER);
  }
}
