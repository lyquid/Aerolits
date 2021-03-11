#ifndef KTP_SDL2_WRAPPERS_SDL2_MUSIC_H_
#define KTP_SDL2_WRAPPERS_SDL2_MUSIC_H_

#include "sdl2_log.hpp"
#include <SDL_mixer.h>
#include <memory>
#include <string>

namespace ktp {

template <typename T> struct deleter;

template <> struct deleter<Mix_Music> {
  void operator()(Mix_Music* music) { 
    Mix_FreeMusic(music);
    music = nullptr;
  }
};

class SDL2_Music {
 public:

  /**
  * Load music file to use. This can load WAVE, MOD, MIDI, OGG, MP3, FLAC, and 
  * any file that you use a command to play with. 
  * If you are using an external command to play the music, you must call Mix_SetMusicCMD 
  * before this, otherwise the internal players will be used. Alternatively, if 
  * you have set an external command up and don't want to use it, you must call
  * Mix_SetMusicCMD(NULL) to use the built-in players again.
  * @param path Path to the music file to use.
  * @return True on success, or false on errors.
  */
  bool loadMusic(const std::string& path);

  /**
  * Play the loaded music loop times through from start to finish. 
  * The previous music will be halted, or if fading out it waits (blocking) for
  * that to finish.
  * @param loops Number of times to play through the music. 0 plays the music zero
  *         times... -1 plays the music forever (or as close as it can get to that).
  * @return True on success, or false on errors.
  */
  bool play(int loops = -1);

  /**
  * Pause the music playback. You may halt paused music.
  * Note: Music can only be paused if it is actively playing.
  */
  static void pause() { Mix_PauseMusic(); }

  /**
  * Unpause the music. This is safe to use on halted, paused, and already playing
  * music.
  */
  static void resume() { Mix_ResumeMusic(); }

  /**
  * Halt playback of music. This interrupts music fader effects. 
  * Any callback set by Mix_HookMusicFinished will be called when the music stops.
  */
  static void stop() { Mix_HaltMusic(); }

  /**
  * Tells you if music is actively playing, or not. 
  * Note: Does not check if the channel has been paused.
  * @return False if the music is not playing, or true if it is playing.
  */
  static bool isPlayingMusic() { return Mix_PlayingMusic() != 0; }

  /**
  * Tells you if music is paused, or not. 
  * Note: Does not check if the music was been halted after it was paused, which 
  * may seem a little weird.
  * @return False if music is not paused. True if it is paused.
  */
  static bool isMusicPaused() { return Mix_PausedMusic() != 0; }

 private:
 
  template <typename T>
  using unique_ptr_deleter = std::unique_ptr<T, deleter<T>>;

  unique_ptr_deleter<Mix_Music> music_{nullptr};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_MUSIC_H_