#include "sdl2_music.hpp"

bool ktp::SDL2_Music::loadMusic(const std::string& path) {
  music_.reset(Mix_LoadMUS(path.c_str()));
  if (music_ == nullptr) {
    logSDL2Error("Mix_LoadMUS", path);
    return false;
  }
  return true;
}

bool ktp::SDL2_Music::play(int loops) {
  if (Mix_PlayMusic(music_.get(), loops) != 0) {
    logSDL2Error("Mix_PlayMusic");
    return false;
  } 
  return true;
}
