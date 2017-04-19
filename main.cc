#include <iostream>
#include <ctime>
#include <string>
#include <opencv2/core/core.hpp>
#include <fstream>
#include <algorithm>
#include <vector>
#include <memory>
#include <chrono>

#include "src/object.h"
#include "src/ray.h"
#include "src/common.h"
#include "src/benchmark.h"
#include "src/file_io.h"
#include "src/camera.h"
#include "src/sampling.h"
#include "src/viewing.h"
#include "src/scene.h"
#include "src/renderer.h"

using namespace render;

int main() {
  Eigen::initParallel();
  auto start = MilliSecondsSinceEpoch();

  auto viewport = std::make_shared<Viewport>(512, 512);

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

  cv::Mat image;
//  Render(scene, viewport, depth_rgb, &image);
  Renderer renderer(viewport);
  renderer.UseMultiJitteredSampling(12);

  FrustumParams frustum;
  frustum.near = 0.1;
  frustum.far = 10;

  auto world = std::make_shared<World>();
  world->AddScene(scene);

  auto camera = std::shared_ptr<OrthographicCamera>(new OrthographicCamera({0, 0, -1}, {0, 0, 0}, {0, -1, 0}, frustum));
  renderer.RenderScene(world, camera, depth_rgb, &image);

  std::string outfile = "/home/daeyun/Dropbox/git/cs419_cpp/out/1.png";
  SavePng(outfile, image);

  std::string outfile2 = "/home/daeyun/Dropbox/git/cs419_cpp/out/1.bin";
  SerializeRgba(outfile2, image);

  auto end = MilliSecondsSinceEpoch();

  std::cout << "Time elapsed: " << end - start << " ms" << std::endl;

  start = MilliSecondsSinceEpoch();
  auto sampler = HemisphereSampler(10.0);
  double pdf;
  Points3d hemi(3, 200000);
  for (int i = 0; i < 200000; ++i) {
    Vec3 sampled;
    sampler.Sample({-1, -1, -1}, &sampled, &pdf);
    hemi.col(i).array() = sampled;
  }
  end = MilliSecondsSinceEpoch();
  std::cout << "Time elapsed: " << end - start << " ms" << std::endl;

  SerializeMatrix("/home/daeyun/Dropbox/git/cs419_cpp/out/2.bin", hemi);

  return 0;
}
