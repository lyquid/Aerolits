#ifndef KTP_SRC_INCLUDE_DEBUG_DRAW_HPP_
#define KTP_SRC_INCLUDE_DEBUG_DRAW_HPP_

#include "opengl.hpp"
#include "resources.hpp"
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <memory>

namespace ktp {

inline constexpr auto kMaxVertices {512u};

class GLRenderLines {
 public:

  GLRenderLines();
  void addVertex(const b2Vec2& vertex, const b2Color& color);
  void update();

 private:

  void updateMVP();

	b2Vec2 vertices_[kMaxVertices * 2];
	b2Color colors_[kMaxVertices * 2];

	int32 count_ {0};

	VAO vao_ {};
  VBO vertices_attr_ {};
  VBO colors_attr_ {};
	ShaderProgram shader_ {Resources::getShader("debug_draw_lines")};
  glm::mat4 mvp_ {};
};

class GLRenderPoints {
 public:

  GLRenderPoints();
  void addVertex(const b2Vec2& vertex, const b2Color& color, float size);
  void update();

 private:

  void updateMVP();

	b2Vec2 vertices_[kMaxVertices];
	b2Color colors_[kMaxVertices];
  float sizes_[kMaxVertices];

	int32 count_ {0};

	VAO vao_ {};
  VBO vertices_attr_ {};
  VBO colors_attr_ {};
  VBO sizes_attr_ {};
	ShaderProgram shader_ {Resources::getShader("debug_draw_points")};
  glm::mat4 mvp_ {};
};

class GLRenderTriangles {
 public:

  GLRenderTriangles();
  void addVertex(const b2Vec2& vertex, const b2Color& color);
  void update();

 private:

  void updateMVP();

	b2Vec2 vertices_[kMaxVertices * 3];
	b2Color colors_[kMaxVertices * 3];

	int32 count_ {0};

	VAO vao_ {};
  VBO vertices_attr_ {};
  VBO colors_attr_ {};
	ShaderProgram shader_ {Resources::getShader("debug_draw_triangles")};
  glm::mat4 mvp_ {};
};

/**
 * @brief Debug draw class heavily based on Erin Catto's testbed implementation. Thanks!
 */
class DebugDraw: public b2Draw {
 public:

  void Draw();
  void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
  void DrawPoint(const b2Vec2& point, float size, const b2Color& color) override;
  void DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) override;
  void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
  void DrawSolidPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) override;
  void DrawSegment(const b2Vec2& start, const b2Vec2& end, const b2Color& color) override;
  void DrawTransform(const b2Transform& xf) override;
  void Init();

 private:

  std::unique_ptr<GLRenderLines>     lines_ {};
  std::unique_ptr<GLRenderPoints>    points_ {};
  std::unique_ptr<GLRenderTriangles> triangles_ {};
};

} // namespace ktp

#endif // KTP_SRC_INCLUDE_DEBUG_DRAW_HPP_
