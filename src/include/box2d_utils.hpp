#ifndef AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_
#define AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_

#include <glm/glm.hpp>
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

  static glm::vec3 convertWorldToScreen(const b2Vec2& pw) {
    float w = 1366;
    float h = 768;
    b2Vec2 m_center {0.f, 20.f}; //?
    float ratio = w / h;
    b2Vec2 extents(ratio * 25.0f, 25.0f);
    extents *= 1.f; // m_zoom ?

    b2Vec2 lower = m_center - extents;
    b2Vec2 upper = m_center + extents;

    float u = (pw.x - lower.x) / (upper.x - lower.x);
    float v = (pw.y - lower.y) / (upper.y - lower.y);

    return glm::vec3{u * w, (1.f - v) * h, 0.f};
  }

} // namespace ktp

#endif // AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_
