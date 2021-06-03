#ifndef AEROLITS_SRC_INCLUDE_RESOURCES_PATH_HPP_
#define AEROLITS_SRC_INCLUDE_RESOURCES_PATH_HPP_

#include "../../sdl2_wrappers/sdl2_wrappers.hpp"
#include <SDL.h>
#include <string>

namespace ktp {

/* Original idea from Will Usher */
/* Check it here: https://github.com/Twinklebear/TwinklebearDev-Lessons */

inline std::string getResourcesPath(const std::string& sub_dir = "") {

  constexpr auto kPROJECT_NAME = "Aerolits";
  constexpr auto kRESOURCES_FOLDER = "resources";

  #ifdef _WIN32
    constexpr auto kPATH_SEPARATOR = '\\';
  #else
    constexpr auto kPATH_SEPARATOR = '/';
  #endif

  std::string base_res{};

  if (base_res.empty()) {

    const auto base_path = SDL_GetBasePath();

    if (base_path) {
      base_res = base_path;
      SDL_free(base_path);
    } else {
      logSDL2Error("SDL_GetBasePath");
      return std::string();
    }

    /* #ifdef _DEBUG
      size_t pos = base_res.rfind("Debug");
    #else
      size_t pos = base_res.rfind("Release");
    #endif  */
    // auto pos = base_res.rfind("bin");

    const auto pos = base_res.rfind(kPROJECT_NAME) + SDL_strlen(kPROJECT_NAME);
    base_res = base_res.substr(0, pos) + kPATH_SEPARATOR + kRESOURCES_FOLDER + kPATH_SEPARATOR;
  }
  return sub_dir.empty() ? base_res : base_res + sub_dir + kPATH_SEPARATOR;
}

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_RESOURCES_PATH_HPP_