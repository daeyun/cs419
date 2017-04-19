//
// Created by daeyun on 4/12/17.
//

#include "gtest/gtest.h"
#include "src/matrix_utils.h"
#include "src/common.h"
#include "src/scene.h"
#include "src/object.h"
#include "src/geom3d.h"
#include "src/viewing.h"
#include "src/renderer.h"

using namespace render;

TEST(SceneTest, RenderDepth) {
  auto depth_rgb = [](const Scene &scene, const Intersection &intersection, Vec3 *rgb) {
    rgb->setZero();
    (*rgb)[0] = intersection.t();
  };
  auto normal_rgb = [](const Scene &scene, const Intersection &intersection, Vec3 *rgb) {
    rgb->setZero();
    *rgb = intersection.normal();
  };

  std::shared_ptr<Scene> scene(new Scene(
      {
          std::shared_ptr<Triangle>(new Triangle({-0.7, -0.7, 0.4}, {1, 0, -0.8}, {0, 1, -0.8})),
          std::shared_ptr<Sphere>(new Sphere(0.8, {0.5, 0.5, 0.0})),
      }
  ));

  FrustumParams frustum;
  frustum.left = -1;
  frustum.right = 1;
  frustum.bottom = -1;
  frustum.top = 1;
  frustum.near = 0.1;
  frustum.far = 10;
  auto viewport = std::make_shared<Viewport>(128, 128);
  auto camera = std::shared_ptr<OrthographicCamera>(new OrthographicCamera({0, 0, -1}, {0, 0, 0}, {0, -1, 0}, frustum));

  auto world = std::make_shared<World>();
  world->AddScene(scene);

  cv::Mat image;
  Renderer renderer(viewport);
  renderer.UseMultiJitteredSampling(4);
  renderer.RenderScene(world, camera, depth_rgb, &image);

  int stencil_filled = 0;

  double max_depth = -kInfinity;
  double min_depth = kInfinity;
  double mean_depth = 0;
  int count = 0;
  int n = image.rows * image.cols;
  for (int i = 0; i < n; ++i) {
    double value = image.at<cv::Vec4d>(i)[3];
    EXPECT_LE(0.0, value);
    EXPECT_GE(1.0, value);
    if (value > 0.5) {
      stencil_filled++;

      double depth = image.at<cv::Vec4d>(i)[0];
      if (depth > max_depth) {
        max_depth = depth;
      }
      if (depth < min_depth) {
        min_depth = depth;
      }
      mean_depth += depth;
      count++;
    }
  }

  // 0.46488
  double stentil_ratio = stencil_filled / static_cast<double>(n);
  mean_depth /= count;
  EXPECT_LE(0.45, stentil_ratio);
  EXPECT_GE(0.48, stentil_ratio);

  // 1.29
  EXPECT_LE(1.0, max_depth);
  EXPECT_GE(1.5, max_depth);

  // 0.1
  EXPECT_LE(0.05, min_depth);
  EXPECT_GE(0.15, min_depth);

  // 0.406
  EXPECT_LE(0.4, mean_depth);
  EXPECT_GE(0.43, mean_depth);
}
