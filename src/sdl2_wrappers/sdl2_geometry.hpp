/**
 * @file sdl2_geometry.hpp
 * @author Alex CB (alex@tinet.org)
 * @brief Useful geometric functions.
 * Triangulation functions adapted from https://www.flipcode.com/archives/Efficient_Polygon_Triangulation.shtml
 */

#ifndef KTP_SDL2_WRAPPERS_SDL2_GEOMETRY_HPP_
#define KTP_SDL2_WRAPPERS_SDL2_GEOMETRY_HPP_

#include <SDL.h>
#include <vector>

namespace ktp { namespace Geometry {

using Point = SDL_FPoint;

/**
 * @brief Set of points that define a polygon.
 *  The last point should be equal to the first in order to close the polygon.
 */
using Polygon = std::vector<Point>;

/**
 * @brief A struct representing a triangle.
 */
struct Triangle {
  Triangle() {}
  Triangle(Point ap, Point bp, Point cp): a(ap), b(bp), c(cp) {}
  Point a{}, b{}, c{};
};

/**
 * @brief Calculates the area of a given polygon.
 * @param polygon The polygon to calculate the area.
 * @return The area of the polygon.
 */
float area(const Polygon& polygon);

/**
 * @brief Returns the distance between 2 points.
 * @tparam T A point type, ie: SDL_Point, ktp::Point.
 * @param a The first point.
 * @param b The second point.
 * @return The distance between the 2 points.
 */
template<typename T>
inline auto distanceBetweenPoints(T a, T b) {
  return SDL_sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

/**
 * @brief Checks if a point is inside a given triangle.
 * @param triangle The triangle.
 * @param point The point.
 * @return True if the point is inside the triangle.
 */
bool insideTriangle(const Triangle& triangle, Point point);

/**
 * @brief
 * @tparam T
 * @tparam U
 * @tparam V
 * @tparam W
 * @tparam X
 * @param polygon
 * @param u
 * @param v
 * @param w
 * @param n
 * @param vec
 * @return true
 */
template<typename T, typename U, typename V, typename W, typename X>
bool snip(const Polygon& polygon, T u, U v, V w, W n, const std::vector<X>& vec) {
  constexpr auto kEpsilon {0.0000000001f};

  const Point a {polygon[vec[u]].x, polygon[vec[u]].y};
  const Point b {polygon[vec[v]].x, polygon[vec[v]].y};
  const Point c {polygon[vec[w]].x, polygon[vec[w]].y};

  if (kEpsilon > (((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x)))) return false;

  Point p {};
  for (std::size_t i = 0; i < n; ++i) {
    if(i == u || i == v || i == w) continue;

    p.x = polygon[vec[i]].x;
    p.y = polygon[vec[i]].y;

    if (insideTriangle({a, b, c}, p)) return false;
  }
  return true;
}

/**
 * @brief Divides a polygon in triangles. **Doesn't work with polygons with inner holes.**
 * @param polygon The polygon to triangularize, or tessellate.
 * @param result Where to store the resulting triangles.
 * @param close True if you want the last point of the triangles to equal the first.
 *  Useful for rendering.
 * @return True if all went ok.
 */
bool triangulate(const Polygon& polygon, std::vector<Polygon>& result, bool close = false);

/**
 * @brief Divides a polygon in triangles. **Doesn't work with polygons with inner holes.**
 * @param polygon The polygon to triangularize, or tessellate.
 * @param result Where to store the resulting triangles.
 * @return True if all went ok.
 */
bool triangulate(const Polygon& polygon, std::vector<Triangle>& result);

/**
 * @brief Divides a polygon in triangles. **Doesn't work with polygons with inner holes.**
 * @tparam T Some object resembling a point (x, y), ie: SDL_Point, SDL_FPoint, ktp::Point.
 * @param polygon The polygon to triangularize, or tessellate.
 * @param result Where to store the resulting triangles.
 * @return True if all went ok.
 */
template<typename T>
bool triangulate(const std::vector<T>& polygon, std::vector<T>& result) {
  const auto vertices {polygon.size()};
  if (vertices < 3) return false;

  std::vector<std::size_t> V {};
  V.resize(vertices);

  // we want a counter-clockwise polygon in V
  if (area(polygon) > 0.f) {
    for (auto i = 0u; i < vertices; ++i)
      V[i] = i;
  }
  else {
    for (auto i = 0u; i < vertices; ++i)
      V[i] = (vertices - 1) - i;
  }

  auto nv {vertices};
  // remove nv-2 Vertices, creating 1 triangle every time
  auto count {2 * nv};   // error detection

  for (std::size_t i = 0, v = nv - 1; nv > 2; ++i) {
    // if we loop, it is probably a non-simple polygon
    if (0 >= (count--)) return false; // Triangulate: ERROR - probable bad polygon!

    // three consecutive vertices in current polygon, <u,v,w>
    auto u {v};
    if (nv <= u) u = 0; // previous
    v = u + 1u;
    if (nv <= v) v = 0; // new v
    auto w {v + 1u};
    if (nv <= w) w = 0; // next

    if (snip(polygon, u, v, w, nv, V)) {
      std::size_t a{V[u]}, b{V[v]}, c{V[w]}, s{}, t{};
      // output Triangle
      result.push_back(polygon[a]);
      result.push_back(polygon[b]);
      result.push_back(polygon[c]);
      // remove v from remaining polygon
      for (s = v, t = v + 1; t < nv; ++s, ++t) V[s] = V[t];
      --nv;
      // reset error detection counter
      count = 2 * nv;
    }
  }
  return true;
}

} // namespace Geometry
} // namespace ktp

#endif // KTP_SDL2_WRAPPERS_SDL2_GEOMETRY_HPP_
