#ifndef KTP_SRC_DEBUG_DRAW_HPP_
#define KTP_SRC_DEBUG_DRAW_HPP_

#include "../sdl2_wrappers/sdl2_wrappers.hpp"

#include <box2d/box2d.h>

namespace ktp {

class DebugDraw : public b2Draw {
 public:

  void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);
  void DrawPoint (const b2Vec2 &p, float size, const b2Color &color);
  void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
  void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);
  void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
  void DrawTransform(const b2Transform& xf);

  inline void setRenderer(const SDL2_Renderer* ren) { renderer_ = ren; }

 private:

  const SDL2_Renderer* renderer_ {nullptr};

};

} // namespace ktp

#endif // KTP_SRC_DEBUG_DRAW_HPP_