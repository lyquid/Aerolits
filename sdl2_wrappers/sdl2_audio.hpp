#ifndef KTP_SDL2_WRAPPERS_SDL2_AUDIO_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_AUDIO_HPP_

#include <SDL_mixer.h>
#include <memory>

namespace ktp {

namespace SDL2_Audio {

  struct MixerSpecs {
    int channels = 0;
    Uint16 format = 0;
    std::string format_str {};
    int frequency = 0;
    int times_opened = 0;
  };

  /**
  * Calls Mix_CloseAudio, which shutdowns and cleanups the mixer API.
  * After calling this all audio is stopped, the device is closed, and the SDL_mixer
  * functions should not be used. You may, of course, use Mix_OpenAudio to start
  * the functionality again.
  * Note: This function doesn't do anything until you have called it the same number
  * of times that you called Mix_OpenAudio. You may use Mix_QuerySpec to find out
  * how many times Mix_CloseAudio needs to be called before the device is actually
  * closed.
  *
  * Also calls Mix_Quit any needed times.
  * This function cleans up all dynamically loaded library handles, freeing memory.
  * If support is required again it will be initialized again, either by Mix_Init
  * or loading a sample or some music with dynamic support required. You may call
  * this function when Mix_Load functions are no longer needed for the MIX_INIT_*
  * formats. You should call this function for each time Mix_Init was called, otherwise
  * it may not free all the dynamic library resources until the program ends. This
  * is done so that multiple unrelated modules of a program may call Mix_Init and
  * Mix_Quit without affecting the others performance and needs.
  */
  void closeMixer();

  /**
  * Initialize the mixer API.
  * This must be called before using other functions in this library (SDL_Mixer).
  * SDL must be initialized with SDL_INIT_AUDIO before this call. frequency would
  * be 44100 for 44.1KHz, which is CD audio rate. Most games use 22050, because
  * 44100 requires too much CPU power on older computers. chunk_size is the size
  * of each mixed sample. The smaller this is the more your hooks will be called.
  * If make this too small on a slow system, sound may skip. If made too large,
  * sound effects will lag behind the action more. You want a happy medium for
  * your target computer. You also may make this 4096, or larger, if you are just
  * playing music. MIX_CHANNELS(8) mixing channels will be allocated by default.
  * You may call this function multiple times, however you will have to call Mix_CloseAudio
  * just as many times for the device to actually close. The format will not changed
  * on subsequent calls until fully closed. So you will have to close all the way
  * before trying to open with different format parameters.
  * @param freq Output sampling frequency in samples per second (Hz). You might
  *             use MIX_DEFAULT_FREQUENCY(22050) since that is a good value for most games.
  * @param format Output sample format.
  * @param channels Number of sound channels in output. Set to 2 for stereo, 1
  *                   for mono. This has nothing to do with mixing channels.
  * @param chunk_size Bytes used per output sample.
  * @return True on success, or false on errors.
  */
  bool initMixer(int freq      = MIX_DEFAULT_FREQUENCY,
                Uint16 format  = MIX_DEFAULT_FORMAT,
                int channels   = MIX_DEFAULT_CHANNELS,
                int chunk_size = 1024);

  /**
   * Get the actual audio format in use by the opened audio device. This may or
   * may not match the parameters you passed to Mix_OpenAudio.
   * @return False on error.
   */
  bool queryMixerSpecs();

  /**
   * Querys for the compiled version and the linked version of SDL_mixer libraries.
   */
  void queryMixerVersions();

  inline SDL_version mixer_compiled_version_{};
  inline const SDL_version* mixer_linked_version_;

  inline MixerSpecs mixer_specs_{};

} // end namespace SDL2_Audio

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_AUDIO_HPP_