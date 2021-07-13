#ifndef KTP_SRC_INCLUDE_DEBUG_DRAW_HPP_
#define KTP_SRC_INCLUDE_DEBUG_DRAW_HPP_

#include "box2d_scale.hpp"
#include "../../sdl2_wrappers/sdl2_wrappers.hpp"
#include <box2d/box2d.h>
#include <vector>

namespace ktp {

class DebugDraw: public b2Draw {
 public:

  void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
    logMessage("DrawCircle not implemented");
    renderer_->setDrawColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
  }

  void DrawPoint(const b2Vec2 &p, float size, const b2Color &color) {
    // missing the size param
    renderer_->setDrawColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
    renderer_->drawPoint(p.x * kMetersToPixels, p.y * kMetersToPixels);
  }

  void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    renderer_->setDrawColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
    std::vector<SDL_FPoint> points(vertexCount + 1);
    for (auto i = 0; i < vertexCount; ++i) {
      points[i].x = vertices[i].x * kMetersToPixels;
      points[i].y = vertices[i].y * kMetersToPixels;
    }
    points[vertexCount] = points[0];
    SDL_RenderDrawLinesF(renderer_->getRenderer(), points.data(), vertexCount + 1);
  }

  void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
    logMessage("DrawSolidCircle not implemented");
    renderer_->setDrawColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
  }

  void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
    // not yet solid
    renderer_->setDrawColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
    std::vector<SDL_FPoint> points(vertexCount + 1);
    for (auto i = 0; i < vertexCount; ++i) {
      points[i].x = vertices[i].x * kMetersToPixels;
      points[i].y = vertices[i].y * kMetersToPixels;
    }
    points[vertexCount] = points[0];
    SDL_RenderDrawLinesF(renderer_->getRenderer(), points.data(), vertexCount + 1);
  }

  void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
    renderer_->setDrawColor(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
    renderer_->drawLine(p1.x * kMetersToPixels, p1.y * kMetersToPixels, p2.x * kMetersToPixels, p2.y * kMetersToPixels);
  }

  void DrawTransform(const b2Transform& xf) {
    constexpr float axis_scale {0.4f};

    const b2Vec2 p1 {xf.p};
    b2Vec2 p2 {};

    renderer_->setDrawColor(255, 0, 0, 0);
    p2 = p1 + axis_scale * xf.q.GetXAxis();
    renderer_->drawLine(p1.x * kMetersToPixels, p1.y * kMetersToPixels, p2.x * kMetersToPixels, p2.y * kMetersToPixels);

    renderer_->setDrawColor(0, 255, 0, 0);
    p2 = p1 - axis_scale * xf.q.GetYAxis();
    renderer_->drawLine(p1.x * kMetersToPixels, p1.y * kMetersToPixels, p2.x * kMetersToPixels, p2.y * kMetersToPixels);
  }

  inline void setRenderer(const SDL2_Renderer* ren) { renderer_ = ren; }

 private:

  const SDL2_Renderer* renderer_ {nullptr};

};

} // namespace ktp

#endif // KTP_SRC_INCLUDE_DEBUG_DRAW_HPP_