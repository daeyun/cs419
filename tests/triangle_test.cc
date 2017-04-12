//
// Created by daeyun on 4/11/17.
//

#include "gtest/gtest.h"

#include "src/object.h"
#include "src/benchmark.h"

using namespace render;

TEST(TriangleTest, Area) {
  auto triangle = Triangle({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
  EXPECT_DOUBLE_EQ(0.5, triangle.area());

  triangle.Translate({0.4, -0.3, -1});
  EXPECT_DOUBLE_EQ(0.5, triangle.area());
}

TEST(TriangleTest, PointInTriangle) {
  auto triangle = Triangle({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
  EXPECT_TRUE(triangle.IsPointInTriangle({0, 0, 0}));
  EXPECT_TRUE(triangle.IsPointInTriangle({1, 0, 0}));
  EXPECT_TRUE(triangle.IsPointInTriangle({0, 1, 0}));
  EXPECT_TRUE(triangle.IsPointInTriangle({0.4, 0.4, 0}));
  EXPECT_FALSE(triangle.IsPointInTriangle({0, 1.01, 0}));
  EXPECT_FALSE(triangle.IsPointInTriangle({0.4, 0.4, 0.1}));
  EXPECT_FALSE(triangle.IsPointInTriangle({0.0, 0.0, 0.1}));

  triangle.Translate({0, 0, -1});

  Vec3 origin(0.2, 0.2, -10);
  Vec3 direction(0, 0, 1);

  auto ray = std::make_shared<Ray>(origin, direction);
  auto point = ray->Point(9);
  EXPECT_TRUE(triangle.IsPointInTriangle(point));
}

