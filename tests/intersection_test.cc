//
// Created by daeyun on 4/11/17.
//

#include "gtest/gtest.h"

#include "src/object.h"

using namespace render;

TEST(IntersectionTest, Sphere) {
  auto sphere = Sphere(3, {1, 1, 1});

  Vec3 origin(1, 1, -10);
  Vec3 direction(0, 0, 1);

  auto ray = std::make_shared<Ray>(origin, direction);

  Intersection intersection(ray);
  sphere.IntersectRay(ray, &intersection);

  auto point = ray->Point(intersection.t());

  EXPECT_EQ(8, intersection.t());

  EXPECT_EQ(1, point[0]);
  EXPECT_EQ(1, point[1]);
  EXPECT_EQ(-2, point[2]);
}

TEST(IntersectionTest, Triangle) {
  auto triangle = Triangle({0, 0, 0}, {1, 0, 0}, {0, 1, 0});
  triangle.Translate({0, 0, -1});

  EXPECT_FLOAT_EQ(0.5, triangle.area());

  Vec3 origin(0.2, 0.2, -10);
  Vec3 direction(0, 0, 1);

  auto ray = std::make_shared<Ray>(origin, direction);

  Intersection intersection(ray);
  bool intersect = triangle.IntersectRay(ray, &intersection);
  EXPECT_TRUE(intersect);

  auto point = ray->Point(intersection.t());

  EXPECT_EQ(9, intersection.t());
}

TEST(IntersectionTest, TriangleNotIntersecting) {
  auto triangle = Triangle({0, 0, 0}, {1, 0, 0}, {0, 1, 0});

  Vec3 origin(0.5, 0.54, -10);
  Vec3 direction(0, 0, 1);

  auto ray = std::make_shared<Ray>(origin, direction);

  Intersection intersection(ray);
  bool intersect = triangle.IntersectRay(ray, &intersection);
  EXPECT_FALSE(intersect);
}

TEST(IntersectionTest, TransformedTriangleIntersection) {
  auto triangle = Triangle({0, 0, 0}, {1, 0, 0}, {0, 1, 0});

  Vec3 origin(0.5, 0.54, -10);
  Vec3 direction(0, 0, 1);

  auto ray = std::make_shared<Ray>(origin, direction);

  Intersection intersection(ray);
  bool intersect = triangle.IntersectRay(ray, &intersection);
  EXPECT_FALSE(intersect);
}

