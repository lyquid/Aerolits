#ifndef KTP_SDL2_WRAPPERS_SDL2_SOUND_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_SOUND_HPP_

#include <SDL_mixer.h>
#include <memory>
#include <string>

namespace ktp {

/* https://stackoverflow.com/questions/29035711/unique-ptr-custom-deleter-and-rule-of-zero */

template <typename T> struct deleter;

template <> struct deleter<Mix_Chunk> {
  void operator()(Mix_Chunk* chunk) { 
    Mix_FreeChunk(chunk);
    chunk = nullptr;
  }
};

class SDL2_Sound {
 public:
 
  bool loadSound(const std::string& path);
  int play(int loops = 0);

 private:

  template <typename T>
  using unique_ptr_deleter = std::unique_ptr<T, deleter<T>>;

  unique_ptr_deleter<Mix_Chunk> sound_{nullptr};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_SOUND_HPP_