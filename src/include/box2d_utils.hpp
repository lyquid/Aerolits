#ifndef AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_
#define AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_

#include <box2d/box2d.h>
#include <SDL.h>

namespace ktp {

inline constexpr float kMetersToPixels {50.f};
inline constexpr float kPixelsToMeters {1.f / kMetersToPixels};

static void b2ColorToSDL2Color(const b2Color& orig, SDL_Color& dest) {
  dest.r = static_cast<Uint8>(orig.r * 255.f);
  dest.g = static_cast<Uint8>(orig.g * 255.f);
  dest.b = static_cast<Uint8>(orig.b * 255.f);
  dest.a = static_cast<Uint8>(orig.a * 255.f);
}

static SDL_Color b2ColorToSDL2Color(const b2Color& orig) {
  return SDL_Color {
    static_cast<Uint8>(orig.r * 255.f),
    static_cast<Uint8>(orig.g * 255.f),
    static_cast<Uint8>(orig.b * 255.f),
    static_cast<Uint8>(orig.a * 255.f)
  };
}

static void SDL2ColorToB2Color(const SDL_Color& orig, b2Color& dest) {
  constexpr float inv {1.f / 255.f};
  dest.r = orig.r * inv;
  dest.g = orig.g * inv;
  dest.b = orig.b * inv;
  dest.a = orig.a * inv;
}

static b2Color SDL2ColorToB2Color(const SDL_Color& orig) {
  constexpr float inv {1.f / 255.f};
  return b2Color {orig.r * inv, orig.g * inv, orig.b * inv, orig.a * inv};
}

/**
 * @brief Removes alll the bodies rom the world.
 * @param world The world to purge.
 */
inline void clearB2World(b2World& world) {
  if (world.GetBodyCount()) {
    b2Body* body {world.GetBodyList()};
    b2Body* aux {nullptr};
    while (body) {
      aux = body->GetNext();
      world.DestroyBody(body);
      body = aux;
    }
  }
}

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_
