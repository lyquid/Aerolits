#ifndef KTP_SDL2_WRAPPERS_SDL2_IMAGE_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_IMAGE_HPP_

#include "sdl2_log.hpp"
#include <SDL.h>
#include <SDL_image.h>

namespace ktp {

class SDL2_Image {
 public:

  static void closeImage() { IMG_Quit(); }

  static bool initImage() {
    queryImageVersions();
    logMessage("SDL_image compiled version: " +
                std::to_string(image_compiled_version_.major) + '.' +
                std::to_string(image_compiled_version_.minor) + '.' +
                std::to_string(image_compiled_version_.patch));
    logMessage("SDL_image linked version: " +
                std::to_string(image_linked_version_->major) + '.' +
                std::to_string(image_linked_version_->minor) + '.' +
                std::to_string(image_linked_version_->patch));

    // Flags available: IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF
    constexpr auto flags = IMG_INIT_PNG;
    const auto initted = IMG_Init(flags);
    if((initted & flags) != flags) {
      logSDL2Error("IMG_Init");
      return false;
    }
    return true;
  }

 private:

  static void queryImageVersions() {
    SDL_IMAGE_VERSION(&image_compiled_version_);
    image_linked_version_ = IMG_Linked_Version();
  }

  inline static       SDL_version image_compiled_version_ {};
  inline static const SDL_version* image_linked_version_ {nullptr};
};

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_IMAGE_HPP_