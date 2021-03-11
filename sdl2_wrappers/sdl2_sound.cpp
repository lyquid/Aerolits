#include "sdl2_sound.hpp"

bool ktp::SDL2_Sound::loadSound(const std::string& path) {
  sound_.reset(Mix_LoadWAV(path.c_str()));
  if (sound_ == nullptr) {
    ktp::logSDL2Error("Mix_LoadWAV", path);
    return false;
  }
  return true;
}

int ktp::SDL2_Sound::play(int loops) {
  return Mix_PlayChannel(-1, sound_.get(), loops);
}
