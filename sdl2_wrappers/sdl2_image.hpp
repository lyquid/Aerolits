#ifndef KTP_SDL2_WRAPPERS_SDL2_IMAGE_H_
#define KTP_SDL2_WRAPPERS_SDL2_IMAGE_H_

#include "sdl2_log.hpp"
#include <SDL.h>
#include <SDL_image.h>

namespace ktp {

namespace SDL2_Image {

  void closeImage();
  bool initImage();
  void queryImageVersions();

  inline SDL_version image_compiled_version_{};
  inline const SDL_version* image_linked_version_;

} // end namespace SDL2_Image

} // end namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_IMAGE_H_