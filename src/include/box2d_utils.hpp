#ifndef AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_
#define AEROLITS_SRC_INCLUDE_BOX2D_UTILS_HPP_

#include <box2d/box2d.h>

namespace ktp {

inline constexpr float kMetersToPixels {50.f};
inline constexpr float kPixelsToMeters {1.f / kMetersToPixels};

/**
 * @brief Removes all the bodies rom the world.
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
