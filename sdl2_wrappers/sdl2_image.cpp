#include "sdl2_image.hpp"
#include "sdl2_log.hpp"

void ktp::SDL2_Image::closeImage() { 
  IMG_Quit();
}

bool ktp::SDL2_Image::initImage() {
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

void ktp::SDL2_Image::queryImageVersions() {
  SDL_IMAGE_VERSION(&image_compiled_version_);
  image_linked_version_ = IMG_Linked_Version();
}