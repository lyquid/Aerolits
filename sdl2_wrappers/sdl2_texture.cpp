#include "sdl2_texture.h"

bool ktp::SDL2_Texture::loadFromFile(const std::string& path) {
  const auto surface = IMG_Load(path.c_str());
  if (surface == nullptr) {
    ktp::logSDL2Error("IMG_Load", path);
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextBlended(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& color) {
  const auto surface = TTF_RenderText_Blended(font.getFont(), text.c_str(), color);
  if (surface == nullptr) {
    ktp::logSDL2Error("TTF_RenderText_Blended");
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextShaded(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& fg_color, const SDL_Color& bg_color) {
  const auto surface = TTF_RenderText_Shaded(font.getFont(), text.c_str(), fg_color, bg_color);
  if (surface == nullptr) {
    ktp::logSDL2Error("TTF_RenderText_Shaded");
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::loadFromTextSolid(const ktp::SDL2_Font& font, const std::string& text, const SDL_Color& color) {
  const auto surface = TTF_RenderText_Solid(font.getFont(), text.c_str(), color);
  if (surface == nullptr) {
    ktp::logSDL2Error("TTF_RenderText_Solid");
    return false;
  } 
  createTextureFromSurface(*surface);
  SDL_FreeSurface(surface);
  return texture_ != nullptr;
}

bool ktp::SDL2_Texture::render(const SDL_Point& where) {
  const SDL_Rect dest = {where.x, where.y, width_, height_};
  if (SDL_RenderCopy(renderer_->getRenderer(), texture_.get(), NULL, &dest) != 0) {
    ktp::logSDL2Error("SDL_RenderCopy");
    return false;
  }
  return true;
}

void ktp::SDL2_Texture::createTextureFromSurface(SDL_Surface& surface) {
  texture_.reset(SDL_CreateTextureFromSurface(renderer_->getRenderer(), &surface));
  if (texture_ == nullptr) {
    ktp::logSDL2Error("SDL_CreateTextureFromSurface");
  } else {
    height_ = surface.h;
    width_ = surface.w;
  }
}
