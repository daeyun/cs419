//
// Created by daeyun on 4/11/17.
//

#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <cstdlib>
#include "src/matrix_utils.h"
#include "src/common.h"
#include "src/viewing.h"
#include "src/camera.h"

using namespace render;

TEST(SimpleTest, SimpleTest) {
  Viewport viewport(2, 2);
  Points2d points;
  viewport.GetPixelCenters(&points);

  EXPECT_EQ(4, points.cols());
  EXPECT_DOUBLE_EQ(-0.5, points.minCoeff());
  EXPECT_DOUBLE_EQ(0.5, points.maxCoeff());
  EXPECT_DOUBLE_EQ(0.5, points.cwiseAbs().minCoeff());
}

TEST(CameraTest, PointTransformation) {
  Vec3 point(0.0, 0.0, 0.0);

  Vec3 campos(0.0, 0.0, 2.0);
  Vec3 lookat(0.0, 0.0, 0.0);
  Vec3 up(0.0, -1.0, 0.0);

  FrustumParams frustum;
  OrthographicCamera cam(campos, lookat, up, frustum);

  Vec3 out;
  cam.WorldToCam(point, &out);

  EXPECT_NEAR(0.0, out[0], 1e-11);
  EXPECT_NEAR(0.0, out[1], 1e-11);
  EXPECT_NEAR(-2.0, out[2], 1e-11);

  Vec3 out2;
  cam.CamToWorld(out, &out2);

  EXPECT_NEAR(0.0, out2[0], 1e-11);
  EXPECT_NEAR(0.0, out2[1], 1e-11);
  EXPECT_NEAR(0.0, out2[2], 1e-11);

  Vec3 point2(0.3, 0.5, 1.0);
  cam.WorldToCam(point2, &out);

  EXPECT_NEAR(-0.3, out[0], 1e-11);
  EXPECT_NEAR(-0.5, out[1], 1e-11);
  EXPECT_NEAR(-1.0, out[2], 1e-11);
}

TEST(CameraTest, NormalTransformation) {
  std::srand((unsigned int) time(0));

  Vec3 point1;
  Vec3 point2;
  Vec3 campos;
  Vec3 lookat;
  Vec3 up;

  for (int i = 0; i < 100; ++i) {

    point1.setRandom();
    point2.setRandom();
    auto normal = (point1 - point2);
    auto norm = normal.norm();

    campos.setRandom();
    lookat.setRandom();
    up.setRandom();

    FrustumParams frustum;
    OrthographicCamera cam(campos, lookat, up, frustum);

    Vec3 out1;
    Vec3 out2;

    Vec3 out;
    cam.WorldToCam(point1, &out1);
    cam.WorldToCam(point2, &out2);

    Vec3 out_normal = out1 - out2;

    Vec3 out_normal2;
    cam.WorldToCamNormal(normal, &out_normal2);

    EXPECT_NEAR(out_normal[0], out_normal2[0], 1e-10);
    EXPECT_NEAR(out_normal[1], out_normal2[1], 1e-10);
    EXPECT_NEAR(out_normal[2], out_normal2[2], 1e-10);

    Vec3 out_normal3;
    cam.CamToWorldNormal(out_normal2, &out_normal3);

    EXPECT_NEAR(normal[0], out_normal3[0], 1e-10);
    EXPECT_NEAR(normal[1], out_normal3[1], 1e-10);
    EXPECT_NEAR(normal[2], out_normal3[2], 1e-10);

    EXPECT_NEAR(norm, out_normal.norm(), 1e-10);
    EXPECT_NEAR(norm, out_normal2.norm(), 1e-10);
    EXPECT_NEAR(norm, out_normal3.norm(), 1e-10);
  }
}

TEST(CameraTest, PerspectiveProjection) {
  std::srand((unsigned int) time(0));

  Vec3 campos = {0, 0, 0};
  Vec3 lookat = {0, 0, -1};
  Vec3 up = {0, 1, 0};

  Vec3 point1;

  FrustumParams frustum;
  frustum.near = 3;
  frustum.far = 9;

  frustum.left = -1;
  frustum.right = 1;

  PerspectiveCamera cam(campos, lookat, up, frustum);

  Vec3 out1;
  Vec3 out2;

  point1 = {0.5, 0.5, -3};
  cam.CamToFrustum(point1, &out1);
  EXPECT_NEAR(0.5, out1[0], 1e-10);
  EXPECT_NEAR(0.5, out1[1], 1e-10);
  EXPECT_NEAR(-1, out1[2], 1e-10);

  point1 = {0.5, 0.5, -6};
  cam.CamToFrustum(point1, &out1);
  EXPECT_NEAR(0.25, out1[0], 1e-10);
  EXPECT_NEAR(0.25, out1[1], 1e-10);
  EXPECT_NEAR(0.5, out1[2], 1e-10);

  point1 = {0.5, 0.5, -4};
  cam.CamToFrustum(point1, &out1);
  cam.FrustumToCam(out1, &out2);
  EXPECT_NEAR(point1[0], out2[0], 1e-10);
  EXPECT_NEAR(point1[1], out2[1], 1e-10);
  EXPECT_NEAR(point1[2], out2[2], 1e-10);
}

TEST(CameraTest, OrthographicProjection) {
  Vec3 campos = {0, 0, 0};
  Vec3 lookat = {0, 0, -1};
  Vec3 up = {0, 1, 0};

  Vec3 point1;

  FrustumParams frustum;
  frustum.near = 3;
  frustum.far = 9;

  frustum.left = -2;
  frustum.right = 2;

  OrthographicCamera cam(campos, lookat, up, frustum);

  Vec3 out1;
  Vec3 out2;

  point1 = {0.5, 0.5, -3};
  cam.CamToFrustum(point1, &out1);
  EXPECT_NEAR(0.25, out1[0], 1e-10);
  EXPECT_NEAR(0.5, out1[1], 1e-10);

  point1 = {0.5, 0.5, -6};
  cam.CamToFrustum(point1, &out1);
  EXPECT_NEAR(0.25, out1[0], 1e-10);
  EXPECT_NEAR(0.5, out1[1], 1e-10);

  point1 = {0.5, 0.5, -4};
  cam.CamToFrustum(point1, &out1);
  cam.FrustumToCam(out1, &out2);
  EXPECT_NEAR(point1[0], out2[0], 1e-10);
  EXPECT_NEAR(point1[1], out2[1], 1e-10);
  EXPECT_NEAR(point1[2], out2[2], 1e-10);
}

TEST(CameraTest, RayTransformation) {
  std::srand((unsigned int) time(0));

  Vec3 campos = {0, 0, 0};
  Vec3 lookat = {0, 0, -1};
  Vec3 up = {0, 1, 0};

  FrustumParams frustum;
  frustum.near = 3;
  frustum.far = 10;
  frustum.left = -1;
  frustum.right = 1;
  frustum.bottom = -1;
  frustum.top = 1;

  PerspectiveCamera cam(campos, lookat, up, frustum);

  auto ray = Ray::FromCamera(0.5, 0.5, cam);

  EXPECT_NEAR(0.5, ray->origin()[0], 1e-10);
  EXPECT_NEAR(0.5, ray->origin()[1], 1e-10);
  EXPECT_NEAR(-3, ray->origin()[2], 1e-10);

  auto d = Vec3(0.5, 0.5, -3).normalized();
  EXPECT_NEAR(d[0], ray->direction()[0], 1e-10);
  EXPECT_NEAR(d[1], ray->direction()[1], 1e-10);
  EXPECT_NEAR(d[2], ray->direction()[2], 1e-10);

  OrthographicCamera cam2(campos, lookat, up, frustum);

  ray = Ray::FromCamera(0.5, 0.5, cam2);

  EXPECT_NEAR(0.5, ray->origin()[0], 1e-10);
  EXPECT_NEAR(0.5, ray->origin()[1], 1e-10);
  EXPECT_NEAR(-3, ray->origin()[2], 1e-10);

  EXPECT_NEAR(0, ray->direction()[0], 1e-10);
  EXPECT_NEAR(0, ray->direction()[1], 1e-10);
  EXPECT_NEAR(-1, ray->direction()[2], 1e-10);
}

