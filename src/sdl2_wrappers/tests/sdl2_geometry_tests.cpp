#include "../sdl2_geometry.hpp"
#include <gtest/gtest.h>
#include <box2d/box2d.h>
#include <vector>

using Line     = ktp::Geometry::Line<SDL_Point>;
using LineF    = ktp::Geometry::Line<SDL_FPoint>;
using LineB2   = ktp::Geometry::Line<b2Vec2>;
using Triangle = ktp::Geometry::Triangle;

TEST(LineTests, Constructors) {
  const Line line1 {};
  EXPECT_EQ(line1.begin.x, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(line1.begin.y, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(line1.end.x, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(line1.end.y, 0) << "Uninitialized members should be 0.";

  const Line line2 {{4, 3}, {5, 9}};
  EXPECT_EQ(line2.begin.x, 4) << "Constructor should initialize members correctly.";
  EXPECT_EQ(line2.begin.y, 3) << "Constructor should initialize members correctly.";
  EXPECT_EQ(line2.end.x, 5) << "Constructor should initialize members correctly.";
  EXPECT_EQ(line2.end.y, 9) << "Constructor should initialize members correctly.";

  const LineF lineF1 {{5.3f, 3.1f}, {5.03f, 9.6f}};
  EXPECT_FLOAT_EQ(lineF1.begin.x, 5.3f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineF1.begin.y, 3.1f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineF1.end.x, 5.03f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineF1.end.y, 9.6f) << "Constructor should initialize members correctly.";

  const LineB2 lineb21 {{5.34f, 3.1558f}, {5.063f, 9.658f}};
  EXPECT_FLOAT_EQ(lineb21.begin.x, 5.34f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineb21.begin.y, 3.1558f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineb21.end.x, 5.063f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineb21.end.y, 9.658f) << "Constructor should initialize members correctly.";

  const LineF lineF2 {5.6543f, 723.51f, 5.037f, 59.6601f};
  EXPECT_FLOAT_EQ(lineF2.begin.x, 5.6543f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineF2.begin.y, 723.51f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineF2.end.x, 5.037f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(lineF2.end.y, 59.6601f) << "Constructor should initialize members correctly.";
}

TEST(LineTests, Multiply) {
  const Line line1 {2, 2, 2, 2};
  const Line line2 {line1 * 3};
  EXPECT_EQ(line2.begin.x, 6) << "Multiply should work.";
  EXPECT_EQ(line2.begin.y, 6) << "Multiply should work.";
  EXPECT_EQ(line2.end.x, 6) << "Multiply should work.";
  EXPECT_EQ(line2.end.y, 6) << "Multiply should work.";

  const LineF linef1 {5.f, 5.f, 5.f, 5.f};
  const LineF linef2 {linef1 * 0.5f};
  EXPECT_FLOAT_EQ(linef2.begin.x, 2.5f) << "Multiply should work.";
  EXPECT_FLOAT_EQ(linef2.begin.y, 2.5f) << "Multiply should work.";
  EXPECT_FLOAT_EQ(linef2.end.x, 2.5f) << "Multiply should work.";
  EXPECT_FLOAT_EQ(linef2.end.y, 2.5f) << "Multiply should work.";
}

TEST(TriangleTests, Constructors) {
  const Triangle triangle {};
  EXPECT_EQ(triangle.a.x, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(triangle.a.y, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(triangle.b.x, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(triangle.b.y, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(triangle.c.x, 0) << "Uninitialized members should be 0.";
  EXPECT_EQ(triangle.c.y, 0) << "Uninitialized members should be 0.";

  const Triangle triangle2 {{0, 1}, {2, 5}, {3, 7}};
  EXPECT_EQ(triangle2.a.x, 0) << "Constructor should initialize members correctly.";
  EXPECT_EQ(triangle2.a.y, 1) << "Constructor should initialize members correctly.";
  EXPECT_EQ(triangle2.b.x, 2) << "Constructor should initialize members correctly.";
  EXPECT_EQ(triangle2.b.y, 5) << "Constructor should initialize members correctly.";
  EXPECT_EQ(triangle2.c.x, 3) << "Constructor should initialize members correctly.";
  EXPECT_EQ(triangle2.c.y, 7) << "Constructor should initialize members correctly.";

  const Triangle triangle3 {{0.25f, 1.102f}, {2.3f, 5.7f}, {3.f, 7.847f}};
  EXPECT_FLOAT_EQ(triangle3.a.x, 0.25f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(triangle3.a.y, 1.102f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(triangle3.b.x, 2.3f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(triangle3.b.y, 5.7f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(triangle3.c.x, 3.f) << "Constructor should initialize members correctly.";
  EXPECT_FLOAT_EQ(triangle3.c.y, 7.847f) << "Constructor should initialize members correctly.";
}

TEST(AreaTests, HandlesBadPolygon) {
  ktp::Geometry::Polygon polygon {};
  auto area = ktp::Geometry::area(polygon);
  EXPECT_FLOAT_EQ(area, 0.f) << "Bad polygons should return 0 area.";
  polygon.push_back({0, 0});
  polygon.push_back({1, 1});
  area = ktp::Geometry::area(polygon);
  EXPECT_FLOAT_EQ(area, 0.f) << "Bad polygons should return 0 area.";
}

TEST(AreaTests, CalculatesArea) {
  ktp::Geometry::Polygon polygon {};
  polygon.push_back({0, 0});
  polygon.push_back({3, 0});
  polygon.push_back({3, 2});
  polygon.push_back({0, 2});
  auto area = ktp::Geometry::area(polygon);
  EXPECT_FLOAT_EQ(area, 6.f) << "The area of a 2x3 rectangle should be 6.";
  polygon.clear();
  polygon.push_back({0, 0});
  polygon.push_back({4, 0});
  polygon.push_back({2, 2});
  area = ktp::Geometry::area(polygon);
  EXPECT_FLOAT_EQ(area, (4 * 2) / 2) << "The area of this triangle should be 4.";
}

TEST(DistanceBetweenPointsTests, CalculatesDistance) {
  ktp::Geometry::Point a{1, 1}, b{2, 1};
  auto distance = ktp::Geometry::distanceBetweenPoints(a, b);
  EXPECT_FLOAT_EQ(distance, 1.f) << "The distance between this points should be 1.";
  a = {-1.5f, -1.5f};
  b = {-2.5f, -1.5f};
  distance = ktp::Geometry::distanceBetweenPoints(a, b);
  EXPECT_FLOAT_EQ(distance, 1.f) << "The distance between this points should be 1.";
}

TEST(InsideTriangleTests, IsInsideATriangle) {
  Triangle triangle {{1, 1}, {4, 1}, {2, 4}};
  SDL_FPoint point {0, 0};
  EXPECT_FALSE(ktp::Geometry::insideTriangle(triangle, point)) << "This point shouldn't be inside the triangle.";
  point = {2, 3};
  EXPECT_TRUE(ktp::Geometry::insideTriangle(triangle, point)) << "This point should be inside the triangle.";
}
