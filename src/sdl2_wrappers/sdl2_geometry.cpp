#include "sdl2_geometry.hpp"

float ktp::Geometry::area(const Polygon& polygon) {
  const auto points {polygon.size()};
  auto area {0.f};

  for (std::size_t p = points - 1, q = 0; q < points; p = q++) {
    area += polygon[p].x * polygon[q].y - polygon[q].x * polygon[p].y;
  }

  return area * 0.5f;
}

bool ktp::Geometry::insideTriangle(const Triangle& triangle, Point point) {
  const Point a {triangle.c.x - triangle.b.x, triangle.c.y - triangle.b.y};
  const Point b {triangle.a.x - triangle.c.x, triangle.a.y - triangle.c.y};
  const Point c {triangle.b.x - triangle.a.x, triangle.b.y - triangle.a.y};

  const Point ap {point.x - triangle.a.x, point.y - triangle.a.y};
  const Point bp {point.x - triangle.b.x, point.y - triangle.b.y};
  const Point cp {point.x - triangle.c.x, point.y - triangle.c.y};

  const auto aXbp {a.x * bp.y - a.y * bp.x};
  const auto cXap {c.x * ap.y - c.y * ap.x};
  const auto bXcp {b.x * cp.y - b.y * cp.x};

  return aXbp >= 0.f && bXcp >= 0.f && cXap >= 0.f;
}

bool ktp::Geometry::triangulate(const Polygon& polygon, std::vector<Polygon>& result, bool close) {
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

bool ktp::Geometry::triangulate(const Polygon& polygon, std::vector<Triangle>& result) {
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
      result.push_back({polygon[a], polygon[b], polygon[c]});
      // remove v from remaining polygon
      for (s = v, t = v + 1; t < nv; ++s, ++t) V[s] = V[t];
      --nv;
      // reset error detection counter
      count = 2 * nv;
    }
  }
  return true;
}