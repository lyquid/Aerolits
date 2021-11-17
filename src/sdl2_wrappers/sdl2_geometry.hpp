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
 * @brief 2 points representing a line.
 * @tparam T Some type resembling a point.
 */
template<typename T>
struct Line {
  Line() = default;
  Line(const T& a, const T& b): begin(a), end(b) {}
  Line(float ax, float ay, float bx, float by) {
    begin.x = ax; begin.y = ay;
    end.x   = bx; end.y   = by;
  }
  T begin{}, end{};
};

template<typename T>
Line<T> operator*(const Line<T>& line, float num) {
  return {line.begin.x * num, line.begin.y * num,
          line.end.x   * num, line.end.y   * num};
}

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
 * @tparam T Some type resembling a point.
 * @param polygon The polygon to calculate the area.
 * @return The area of the polygon.
 */
template<typename T>
float area(const std::vector<T>& polygon) {
  const auto points {polygon.size()};
  auto area {0.f};

  for (std::size_t p = points - 1, q = 0; q < points; p = q++) {
    area += polygon[p].x * polygon[q].y - polygon[q].x * polygon[p].y;
  }

  return area * 0.5f;
}

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
 * @brief Calculates a perpendicular line to the given one that passes by the specified point.
 * @tparam T A point type, ie: SDL_Point, ktp::Point.
 * @tparam U A point type, ie: SDL_Point, ktp::Point.
 * @tparam V A point type, ie: SDL_Point, ktp::Point.
 * @param begin The starting point of the line to calculate the perpendicular from.
 * @param end The ending point of the line to calculate the perpendicular from.
 * @param p_begin The starting point of the perpendicular.
 * @param p_end The starting point of the perpendicular.
 * @param passes_by A point that the perpendicular must cross.
 */
template<typename T, typename U, typename V>
void perpendicular(const T& begin, const T& end, U& p_begin, U& p_end, const V& passes_by) {
  p_begin.x = passes_by.x - (end.y - begin.y);
  p_begin.y = passes_by.y + (end.x - begin.x);
  p_end.x   = passes_by.x + (end.y - begin.y);
  p_end.y   = passes_by.y - (end.x - begin.x);
}

/**
 * @brief Calculates a perpendicular line to the given one.
 * @tparam T A point type, ie: SDL_Point, ktp::Point.
 * @tparam U A point type, ie: SDL_Point, ktp::Point.
 * @param begin The starting point of the line to calculate the perpendicular from.
 * @param end The ending point of the line to calculate the perpendicular from.
 * @param p_begin The starting point of the perpendicular.
 * @param p_end The starting point of the perpendicular.
 */
template<typename T, typename U>
inline void perpendicular(const T& begin, const T& end, U& p_begin, U& p_end) {
  perpendicular(begin, end, p_begin, p_end, SDL_FPoint{0.f, 0.f});
}

template<typename T, typename U>
auto perpendicular(const T& begin, const T& end, const U& passes_by) {
  Line<T> pp_line {};
  perpendicular(begin, end, pp_line.begin, pp_line.end, passes_by);
  return pp_line;
}

template<typename T>
auto perpendicular(const T& begin, const T& end) {
  Line<T> pp_line {};
  perpendicular(begin, end, pp_line.begin, pp_line.end, SDL_FPoint{0.f, 0.f});
  return pp_line;
}

template<typename T, typename U>
inline void perpendicular(const Line<T>& line, Line<T>& pp_line, const U& passes_by) {
  perpendicular(line.begin, line.end, pp_line.begin, pp_line.end, passes_by);
}

template<typename T>
inline void perpendicular(const Line<T>& line, Line<T>& pp_line) {
  perpendicular(line.begin, line.end, pp_line.begin, pp_line.end, SDL_FPoint{0.f, 0.f});
}

template<typename T, typename U>
auto perpendicular(const Line<T>& line, const U& passes_by) {
  Line<T> pp_line {};
  perpendicular(line.begin, line.end, pp_line.begin, pp_line.end, passes_by);
  return pp_line;
}

template<typename T>
auto perpendicular(const Line<T>& line) {
  Line<T> pp_line {};
  perpendicular(line.begin, line.end, pp_line.begin, pp_line.end, SDL_FPoint{0.f, 0.f});
  return pp_line;
}

/**
 * @brief
 * @tparam T
 * @tparam U
 * @tparam V
 * @tparam W
 * @tparam X
 * @tparam Y
 * @param polygon
 * @param u
 * @param v
 * @param w
 * @param n
 * @param vec
 * @return true
 */
template<typename T, typename U, typename V, typename W, typename X, typename Y>
bool snip(const std::vector<T>& polygon, U u, V v, W w, X n, const std::vector<Y>& vec) {
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

/**
 * @brief Divides a polygon in triangles. **Doesn't work with polygons with inner holes.**
 * @tparam T Some object resembling a point (x, y), ie: SDL_Point, SDL_FPoint, ktp::Point.
 * @param polygon The polygon to triangularize, or tessellate.
 * @param result Where to store the resulting triangles.
 * @return True if all went ok.
 */
template<typename T>
bool triangulate(const std::vector<T>& polygon, std::vector<std::vector<T>>& result, bool close = false) {
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
      if (close) {
        result.push_back({polygon[a], polygon[b], polygon[c], polygon[a]});
      } else {
        result.push_back({polygon[a], polygon[b], polygon[c]});
      }
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
