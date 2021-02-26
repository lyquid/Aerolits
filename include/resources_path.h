#ifndef KUGE_INCLUDE_RESOURCES_PATH_H
#define KUGE_INCLUDE_RESOURCES_PATH_H

#include "../sdl2_wrappers/sdl2_wrappers.h"
#include <SDL.h>
#include <string>

namespace ktp {

/* Original idea from Will Usher */
/* Check it here: https://github.com/Twinklebear/TwinklebearDev-Lessons */

static std::string getResourcesPath(const std::string& subDir = "") {

  constexpr auto kPROJECT_NAME = "Aerolits";
  constexpr auto kRESOURCES_FOLDER = "resources";

  #ifdef _WIN32
    constexpr auto kPATH_SEPARATOR = '\\';
  #else
    constexpr auto kPATH_SEPARATOR = '/';
  #endif

  static std::string base_res;

  if (base_res.empty()) {

    auto base_path = SDL_GetBasePath();

    if (base_path) {
      base_res = base_path;
      SDL_free(base_path);
    } else {
      ktp::logSDL2Error("SDL_GetBasePath");
      return std::string();
    }

    /* #ifdef _DEBUG
      size_t pos = base_res.rfind("Debug");
    #else
      size_t pos = base_res.rfind("Release");
    #endif  */
    // auto pos = base_res.rfind("bin");

    auto pos = base_res.rfind(kPROJECT_NAME) + SDL_strlen(kPROJECT_NAME);
    base_res = base_res.substr(0, pos) + kPATH_SEPARATOR + kRESOURCES_FOLDER + kPATH_SEPARATOR;
  }
  return subDir.empty() ? base_res : base_res + subDir + kPATH_SEPARATOR;
}

} // namespace ktp
#endif // KUGE_INCLUDE_RESOURCES_PATH_H