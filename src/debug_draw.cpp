#include "include/box2d_utils.hpp"
#include "include/debug_draw.hpp"
#include "include/game.hpp"
#include <glm/gtc/type_ptr.hpp>

// GLRenderLines

ktp::GLRenderLines::GLRenderLines() {
  vao_.bind();

  vertices_attr_.setup(nullptr, sizeof(vertices_), GL_DYNAMIC_DRAW);
  vao_.linkAttrib(vertices_attr_, 0, 2, GL_FLOAT, 0, nullptr);

  colors_attr_.setup(nullptr, sizeof(colors_), GL_DYNAMIC_DRAW);
  vao_.linkAttrib(colors_attr_, 1, 4, GL_FLOAT, 0, nullptr);
}

void ktp::GLRenderLines::addVertex(const b2Vec2& vertex, const b2Color& color) {
  if (count_ == kMaxVertices * 2) update();

  vertices_[count_] = vertex;
  colors_[count_] = color;
  ++count_;
}

void ktp::GLRenderLines::update() {
  if (!count_) return;

  updateMVP();
  shader_.use();
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));

  vao_.bind();
  vertices_attr_.setupSubData(vertices_, count_ * sizeof(b2Vec2));
  colors_attr_.setupSubData(colors_, count_ * sizeof(b2Color));

  glDrawArrays(GL_LINES, 0, count_);

  count_ = 0;
}

void ktp::GLRenderLines::updateMVP() {
  const glm::mat4 model {1.f};
  mvp_ = Game::camera_.projectionMatrix() * Game::camera_.viewMatrix() * model;
}

// GLRenderPoints

ktp::GLRenderPoints::GLRenderPoints() {
  vao_.bind();

  vertices_attr_.setup(nullptr, sizeof(vertices_), GL_DYNAMIC_DRAW);
  vao_.linkAttrib(vertices_attr_, 0, 2, GL_FLOAT, 0, nullptr);

  colors_attr_.setup(nullptr, sizeof(colors_), GL_DYNAMIC_DRAW);
  vao_.linkAttrib(colors_attr_, 1, 4, GL_FLOAT, 0, nullptr);

  sizes_attr_.setup(nullptr, sizeof(sizes_), GL_DYNAMIC_DRAW);
  vao_.linkAttrib(sizes_attr_, 2, 1, GL_FLOAT, 0, nullptr);
}

void ktp::GLRenderPoints::addVertex(const b2Vec2& vertex, const b2Color& color, float size) {
  if (count_ == kMaxVertices) update();

  vertices_[count_] = vertex;
  colors_[count_] = color;
  sizes_[count_] = size;
  ++count_;
}

void ktp::GLRenderPoints::update() {
  if (!count_) return;

  updateMVP();
  shader_.use();
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));

  vao_.bind();
  vertices_attr_.setupSubData(vertices_, count_ * sizeof(b2Vec2));
  colors_attr_.setupSubData(colors_, count_ * sizeof(b2Color));
  sizes_attr_.setupSubData(sizes_, count_ * sizeof(float));

  glEnable(GL_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, count_);
	glDisable(GL_PROGRAM_POINT_SIZE);

  count_ = 0;
}

void ktp::GLRenderPoints::updateMVP() {
  const glm::mat4 model {1.f};
  mvp_ = Game::camera_.projectionMatrix() * Game::camera_.viewMatrix() * model;
}

// GLRenderTriangles

ktp::GLRenderTriangles::GLRenderTriangles() {
  vao_.bind();

  vertices_attr_.setup(nullptr, sizeof(vertices_), GL_DYNAMIC_DRAW);
  vao_.linkAttrib(vertices_attr_, 0, 2, GL_FLOAT, 0, nullptr);

  colors_attr_.setup(nullptr, sizeof(colors_), GL_DYNAMIC_DRAW);
  vao_.linkAttrib(colors_attr_, 1, 4, GL_FLOAT, 0, nullptr);
}

void ktp::GLRenderTriangles::addVertex(const b2Vec2& vertex, const b2Color& color) {
  if (count_ == kMaxVertices * 3) update();

  vertices_[count_] = vertex;
  colors_[count_] = color;
  ++count_;
}

void ktp::GLRenderTriangles::update() {
  if (!count_) return;

  updateMVP();
  shader_.use();
  shader_.setMat4f("mvp", glm::value_ptr(mvp_));

  vao_.bind();
  vertices_attr_.setupSubData(vertices_, count_ * sizeof(b2Vec2));
  colors_attr_.setupSubData(colors_, count_ * sizeof(b2Color));

  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDrawArrays(GL_TRIANGLES, 0, count_);
  // glDisable(GL_BLEND);

  count_ = 0;
}

void ktp::GLRenderTriangles::updateMVP() {
  const glm::mat4 model {1.f};
  mvp_ = Game::camera_.projectionMatrix() * Game::camera_.viewMatrix() * model;
}

// DebugDraw

void ktp::DebugDraw::Draw() {
  lines_->update();
  points_->update();
  triangles_->update();
}

void ktp::DebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color) {
  constexpr float segments {16.f};
	constexpr float increment {2.f * b2_pi / segments};
	const float sin_inc {sinf(increment)};
	const float cos_inc {cosf(increment)};
	b2Vec2 r1 {1.f, 0.f};
	b2Vec2 v1 {center + radius * r1};
	for (int32 i = 0; i < segments; ++i) {
		// Perform rotation to avoid additional trigonometry.
		const b2Vec2 r2 {
		  cos_inc * r1.x - sin_inc * r1.y,
		  sin_inc * r1.x + cos_inc * r1.y
    };
		const b2Vec2 v2 {center + radius * r2};
		lines_->addVertex(kMetersToPixels * v1, color);
		lines_->addVertex(kMetersToPixels * v2, color);
		r1 = r2;
		v1 = v2;
	}
}

void ktp::DebugDraw::DrawPoint(const b2Vec2& point, float size, const b2Color& color) {
  points_->addVertex(kMetersToPixels * point, color, size);
}

void ktp::DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
  b2Vec2 p1 {vertices[vertex_count - 1]};
	for (int32 i = 0; i < vertex_count; ++i) {
		const b2Vec2 p2 {vertices[i]};
		lines_->addVertex(kMetersToPixels * p1, color);
		lines_->addVertex(kMetersToPixels * p2, color);
		p1 = p2;
	}
}

void ktp::DebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) {
  constexpr float segments {16.f};
	constexpr float increment {2.f * b2_pi / segments};
	const float sin_inc {sinf(increment)};
	const float cos_inc {cosf(increment)};
	const b2Vec2 v0 {center};
	b2Vec2 r1 {cos_inc, sin_inc};
	b2Vec2 v1 {center + radius * r1};
	const b2Color fill_color {0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f};
	for (int32 i = 0; i < segments; ++i) {
		// Perform rotation to avoid additional trigonometry.
		const b2Vec2 r2 {
		  cos_inc * r1.x - sin_inc * r1.y,
		  sin_inc * r1.x + cos_inc * r1.y
    };
		const b2Vec2 v2 {center + radius * r2};
		triangles_->addVertex(kMetersToPixels * v0, fill_color);
		triangles_->addVertex(kMetersToPixels * v1, fill_color);
		triangles_->addVertex(kMetersToPixels * v2, fill_color);
		r1 = r2;
		v1 = v2;
	}
	r1.Set(1.f, 0.f);
	v1 = center + radius * r1;
	for (int32 i = 0; i < segments; ++i) {
		const b2Vec2 r2 {
		  cos_inc * r1.x - sin_inc * r1.y,
		  sin_inc * r1.x + cos_inc * r1.y
    };
		const b2Vec2 v2 {center + radius * r2};
		lines_->addVertex(kMetersToPixels * v1, color);
		lines_->addVertex(kMetersToPixels * v2, color);
		r1 = r2;
		v1 = v2;
	}
	// Draw a line fixed in the circle to animate rotation.
	const b2Vec2 p {center + radius * axis};
	lines_->addVertex(kMetersToPixels * center, color);
	lines_->addVertex(kMetersToPixels * p, color);
}

void ktp::DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertex_count, const b2Color& color) {
  const b2Color fill_color {0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f};

	for (int32 i = 1; i < vertex_count - 1; ++i) {
		triangles_->addVertex(kMetersToPixels * vertices[0], fill_color);
		triangles_->addVertex(kMetersToPixels * vertices[i], fill_color);
		triangles_->addVertex(kMetersToPixels * vertices[i + 1], fill_color);
	}

	b2Vec2 p1 {vertices[vertex_count - 1]};
	for (int32 i = 0; i < vertex_count; ++i) {
		b2Vec2 p2 {vertices[i]};
		lines_->addVertex(kMetersToPixels * p1, color);
		lines_->addVertex(kMetersToPixels * p2, color);
		p1 = p2;
	}
}

void ktp::DebugDraw::DrawSegment(const b2Vec2& start, const b2Vec2& end, const b2Color& color) {
  lines_->addVertex(kMetersToPixels * start, color);
	lines_->addVertex(kMetersToPixels * end, color);
}

void ktp::DebugDraw::DrawTransform(const b2Transform& xf) {
  constexpr auto axis_scale {0.4f};
	const b2Color red {1.f, 0.f, 0.f};
	const b2Color green {0.f, 1.f, 0.f};
	b2Vec2 p1 {xf.p}, p2 {};

	lines_->addVertex(kMetersToPixels * p1, red);
	p2 = p1 + axis_scale * xf.q.GetXAxis();
	lines_->addVertex(kMetersToPixels * p2, red);

	lines_->addVertex(kMetersToPixels * p1, green);
	p2 = p1 + axis_scale * xf.q.GetYAxis();
	lines_->addVertex(kMetersToPixels * p2, green);
}

void ktp::DebugDraw::Init() {
  lines_ = std::make_unique<GLRenderLines>();
  points_ = std::make_unique<GLRenderPoints>();
  triangles_ = std::make_unique<GLRenderTriangles>();
}
