#include "sdl2_audio.hpp"

void ktp::SDL2_Audio::closeMixer() {
  Mix_CloseAudio();
  while (Mix_Init(0)) Mix_Quit();
}

bool ktp::SDL2_Audio::initMixer(int freq, Uint16 format, int channels, int chunk_size) {
  queryMixerVersions();
  logMessage("SDL_mixer compiled version: " +  
              std::to_string(mixer_compiled_version_.major) + '.' + 
              std::to_string(mixer_compiled_version_.minor) + '.' + 
              std::to_string(mixer_compiled_version_.patch));
  logMessage("SDL_mixer linked version: " + 
              std::to_string(mixer_linked_version_->major) + '.' + 
              std::to_string(mixer_linked_version_->minor) + '.' + 
              std::to_string(mixer_linked_version_->patch));

  constexpr auto audio_flags = MIX_INIT_OGG | MIX_INIT_MOD;
  const auto initted = Mix_Init(audio_flags);
  if ((initted & audio_flags) != audio_flags) {
    logSDL2Error("Mix_Init");
    return false;
  }
  if (Mix_OpenAudio(freq, format, channels, chunk_size) == -1) {
    logSDL2Error("Mix_OpenAudio");
    while (Mix_Init(0)) Mix_Quit();
    return false;
  }
  if (!queryMixerSpecs()) {
    // can this crash if Mix_OpenAudio was correct?
    logSDL2Error("Mix_QuerySpec");
    // while (Mix_Init(0)) Mix_Quit();
    // return false;
  } else {
    logMessage("Mixer Specs: " + mixer_specs_.format_str + " / " +
                std::to_string(mixer_specs_.frequency) + "Hz / " +
                std::to_string(mixer_specs_.channels) + " channels");
  }
  return true;
}

bool ktp::SDL2_Audio::queryMixerSpecs() {
  mixer_specs_.times_opened = Mix_QuerySpec(&mixer_specs_.frequency, 
                                            &mixer_specs_.format, 
                                            &mixer_specs_.channels);          
  if (mixer_specs_.times_opened) {
    const char* format_str;
    switch(mixer_specs_.format) {
      case AUDIO_U8:     format_str = "U8"; break;
      case AUDIO_S8:     format_str = "S8"; break;
      case AUDIO_U16LSB: format_str = "U16LSB"; break;
      case AUDIO_S16LSB: format_str = "S16LSB"; break;
      case AUDIO_U16MSB: format_str = "U16MSB"; break;
      case AUDIO_S16MSB: format_str = "S16MSB"; break;
      default:           format_str = "Unknown"; break;
    }
    mixer_specs_.format_str = format_str;
    return true;
  }
  return false;
}

void ktp::SDL2_Audio::queryMixerVersions() {
  SDL_MIXER_VERSION(&mixer_compiled_version_);
  mixer_linked_version_ = Mix_Linked_Version();
}