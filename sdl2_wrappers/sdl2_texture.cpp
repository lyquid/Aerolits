#include "sdl2_texture.hpp"

bool ktp::SDL2_Texture::loadFromFile(const std::string& path) {
  const auto surface{IMG_Load(path.c_str())};
  if (surface == nullptr) {
    logSDL2Error("IMG_Load", path);
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextBlended(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& color) {
  const auto surface{TTF_RenderText_Blended(font.getFont(), text.c_str(), color)};
  if (surface == nullptr) {
    logSDL2Error("TTF_RenderText_Blended");
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextShaded(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& fg_color, const SDL_Color& bg_color) {
  const auto surface{TTF_RenderText_Shaded(font.getFont(), text.c_str(), fg_color, bg_color)};
  if (surface == nullptr) {
    logSDL2Error("TTF_RenderText_Shaded");
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextSolid(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& color) {
  const auto surface{TTF_RenderText_Solid(font.getFont(), text.c_str(), color)};
  if (surface == nullptr) {
    logSDL2Error("TTF_RenderText_Solid");
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::render(const SDL_Point& where) {
  const SDL_Rect dest{where.x, where.y, width_, height_};
  if (SDL_RenderCopy(renderer_->getRenderer(), texture_.get(), NULL, &dest) != 0) {
    logSDL2Error("SDL_RenderCopy");
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_FPoint& where) {
  const SDL_Rect dest{static_cast<int>(where.x), static_cast<int>(where.y), width_, height_};
  if (SDL_RenderCopy(renderer_->getRenderer(), texture_.get(), NULL, &dest) != 0) {
    logSDL2Error("SDL_RenderCopy");
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_Rect& src_rect, const SDL_Rect& dest_rect) {
  if (SDL_RenderCopy(renderer_->getRenderer(), texture_.get(), &src_rect, &dest_rect) != 0) {
    logSDL2Error("SDL_RenderCopy");
    return false;
  }
  return true;
}

bool ktp::SDL2_Texture::render(const SDL_Rect& src_rect, const SDL_Rect& dest_rect, float angle) {
  if (SDL_RenderCopyEx(renderer_->getRenderer(), texture_.get(), &src_rect, &dest_rect, angle, NULL, SDL_FLIP_NONE) != 0) {
    logSDL2Error("SDL_RenderCopyEx");
    return false;
  }
  return true;
}

void ktp::SDL2_Texture::createTextureFromSurface(SDL_Surface& surface) {
  texture_.reset(SDL_CreateTextureFromSurface(renderer_->getRenderer(), &surface));
  if (texture_ == nullptr) {
    logSDL2Error("SDL_CreateTextureFromSurface");
  } else {
    height_ = surface.h;
    width_ = surface.w;
  }
}
