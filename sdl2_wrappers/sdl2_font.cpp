#include "sdl2_font.hpp"
#include "sdl2_log.hpp"

bool ktp::SDL2_Font::initTTF() {
  if (!TTF_WasInit() && TTF_Init() != 0) {
    logSDL2Error("TTF_Init");
    return false;
  } else {
    queryTTFVersions();
    logMessage("SDL_ttf compiled version: " +  
                std::to_string(ttf_compiled_version_.major) + '.' + 
                std::to_string(ttf_compiled_version_.minor) + '.' + 
                std::to_string(ttf_compiled_version_.patch));
    logMessage("SDL_ttf linked version: " + 
                std::to_string(ttf_linked_version_->major) + '.' +   
                std::to_string(ttf_linked_version_->minor) + '.' + 
                std::to_string(ttf_linked_version_->patch));
  }
  return true;
}

bool ktp::SDL2_Font::loadFont(const std::string& path, int size) {
  font_.reset(TTF_OpenFont(path.c_str(), size));
  if (font_ == nullptr) {
    logSDL2Error("TTF_OpenFont");
    return false;
  }
  queryFontInfo();
  return true;
}

void ktp::SDL2_Font::setHinting(int hinting) {
  if (font_info_.hinting_ != hinting) {
    TTF_SetFontHinting(font_.get(), hinting);
    queryFontInfo();
  }
}

void ktp::SDL2_Font::setKerning(bool allowed) {
  if (font_info_.kerning_ != allowed) {
    allowed ? TTF_SetFontKerning(font_.get(), 1) : TTF_SetFontKerning(font_.get(), 0);
    queryFontInfo();
  }
}

void ktp::SDL2_Font::setOutline(int pixels) {
  if (font_info_.outline_ != pixels) {
    TTF_SetFontOutline(font_.get(), pixels);
    queryFontInfo();
  }
}

void ktp::SDL2_Font::setStyle(int style) {
  if (font_info_.style_ != style) {
    TTF_SetFontStyle(font_.get(), style);
    queryFontInfo();
  }
}

void ktp::SDL2_Font::queryFontInfo() {
  font_info_.face_family_name_ = TTF_FontFaceFamilyName(font_.get());
  font_info_.face_style_name_  = TTF_FontFaceStyleName(font_.get());
  font_info_.hinting_          = TTF_GetFontHinting(font_.get());
  font_info_.kerning_          = TTF_GetFontKerning(font_.get()) != 0;
  font_info_.line_skip_        = TTF_FontLineSkip(font_.get());
  font_info_.max_ascent_       = TTF_FontAscent(font_.get());
  font_info_.max_descent_      = TTF_FontDescent(font_.get());
  font_info_.max_height_       = TTF_FontHeight(font_.get());
  font_info_.monospaced_       = TTF_FontFaceIsFixedWidth(font_.get()) != 0;
  font_info_.outline_          = TTF_GetFontOutline(font_.get());
  font_info_.style_            = TTF_GetFontStyle(font_.get());
}

void ktp::SDL2_Font::queryTTFVersions() {
  SDL_TTF_VERSION(&ttf_compiled_version_);
  ttf_linked_version_ = TTF_Linked_Version();
}
