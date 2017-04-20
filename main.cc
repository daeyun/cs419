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
#include "src/brdf.h"

using namespace render;

int main() {
  Eigen::initParallel();

  auto viewport = std::make_shared<Viewport>(512, 512);

  auto depth_rgb = [](const World &world, const Intersection &intersection, Vec3 *rgb) {
    rgb->setZero();
    (*rgb)[0] = intersection.t();
  };
  auto normal_rgb = [](const World &world, const Intersection &intersection, Vec3 *rgb) {
    rgb->setZero();
    *rgb = intersection.normal();
  };

  HemisphereSampler hemi_sampler(1.0);

  std::function<void(const World &, const Intersection &, Vec3 *)>
      rt_rgb = [&hemi_sampler, &rt_rgb](const World &world, const Intersection &intersection, Vec3 *rgb) {
    rgb->setZero();

    const auto cam = world.camera();
    const auto cam_direction = (cam->position() - cam->lookat_position()).normalized();

    auto normal = intersection.normal();
    // Render both sides.
    if (normal.dot(cam_direction) < 0) {
      normal *= -1;
    }
    const auto point = intersection.ray()->Point(intersection.t());

    const auto scene = world.scene();
    for (const auto &light: world.lights()) {
      auto direction = light->Direction(point);
      if (normal.dot(direction) < 0) {
        continue;
      }
      auto distance_to_light = (light->position() - point).norm();
      auto light_ray = std::make_shared<Ray>(point, direction);
      Intersection light_intersection(light_ray);
      bool hit = scene->IntersectRay(light_ray, &light_intersection);

      if (!hit) {
        *rgb += Vec3(0.4, 0.4, 0.4) * normal.dot(direction);
      }
    }

    Vec3 L;
    L.setZero();

    const int n = 10;
    for (int i = 0; i < n; ++i) {
      Vec3 sampled;
      double pdf;
      hemi_sampler.Sample(normal, &sampled, &pdf);

      auto light_ray = std::make_shared<Ray>(point, sampled);
      Intersection light_intersection(light_ray);
      bool hit = scene->IntersectRay(light_ray, &light_intersection);

      if (!hit) {
        L += world.ambient() * (normal.dot(sampled) / (pdf * M_PI));
      }
    }
    L /= n;
    *rgb += L;
  };

//  std::shared_ptr<Scene> scene(new Scene(
//      {
//          std::shared_ptr<Triangle>(new Triangle({-0.7, -0.7, 0.4}, {1, 0, -0.8}, {0, 1, -0.8})),
//          std::shared_ptr<Sphere>(new Sphere(0.8, {0.5, 0.5, 0.0})),
//      }
//  ));
  std::shared_ptr<Scene> scene(new Scene(
      {
          std::shared_ptr<Triangle>(new Triangle({1.5, 1.5, 0}, {1.5, -1.5, 0}, {-1.5, 1.5, 0})),
          std::shared_ptr<Triangle>(new Triangle({-1.5, -1.5, 0}, {1.5, -1.5, 0}, {-1.5, 1.5, 0})),
          std::shared_ptr<Triangle>(new Triangle({0, 0, 1}, {1, 0, 0}, {0, 0, 0})),
          std::shared_ptr<Sphere>(new Sphere(0.5, {0.0, 0.0, 0.5})),
          std::shared_ptr<Sphere>(new Sphere(0.3, {1.0, 0.0, 0.3})),
          std::shared_ptr<Sphere>(new Sphere(0.3, {0.0, 1.0, 0.3})),
          std::shared_ptr<Triangle>(new Triangle({-1.3, 1.0, 0}, {-1.3, -1.0, 0}, {-1.3, 1.0, 0.5})),
          std::shared_ptr<Triangle>(new Triangle({-1, 0.8, 0}, {-1, -0.8, 0}, {-1, 0.8, 0.5})),
      }
  ));
//  std::shared_ptr<Scene> scene(new Scene(
//      {
//          std::shared_ptr<Sphere>(new Sphere(0.5, {0.0, 0.0, 0.0})),
//      }
//  ));

  constexpr int kNumRepeats = 1;

  std::vector<long> times;

  cv::Mat image;

  FrustumParams frustum;
  frustum.near = 3.0;
  frustum.far = 10.0;

  auto camera = std::shared_ptr<PerspectiveCamera>(new PerspectiveCamera({3, 3, 3}, {0, 0, 0}, {0, 0, 1}, frustum));

  auto world = std::make_shared<World>();
  world->set_scene(scene);
  world->set_camera(camera);
  world->set_ambient({0.2, 0.2, 0.2});

  world->AddLight(std::shared_ptr<PointLight>(new PointLight({0, 1, 0.9})));

  for (int j = 0; j < kNumRepeats; ++j) {
    auto start = MilliSecondsSinceEpoch();

    cv::Mat img;
    Renderer renderer(viewport);
    renderer.UseMultiJitteredSampling(6);

    renderer.RenderScene(world, camera, rt_rgb, &img);

    auto end = MilliSecondsSinceEpoch();
    times.push_back(end - start);

    if (j == 0) {
      image = img;
    }
  }
  long best_time = *std::min_element(times.begin(), times.end());

  std::cout << "Time elapsed: " << best_time << " ms" << std::endl;

  for (const auto &time: times) {
    std::cout << time << "\t";
  }
  std::cout << std::endl;
  std::cout << std::endl;

  std::string outfile = "/home/daeyun/Dropbox/git/cs419_cpp/out/1.png";
  SavePng(outfile, image);

  std::string outfile2 = "/home/daeyun/Dropbox/git/cs419_cpp/out/1.bin";
  SerializeRgba(outfile2, image);

  auto start = MilliSecondsSinceEpoch();
  auto sampler = HemisphereSampler(10.0);
  double pdf;
  Points3d hemi(3, 200000);
  for (int i = 0; i < 200000; ++i) {
    Vec3 sampled;
    sampler.Sample({-1, -1, -1}, &sampled, &pdf);
    hemi.col(i).array() = sampled;
  }
  auto end = MilliSecondsSinceEpoch();
  std::cout << "Time elapsed: " << end - start << " ms" << std::endl;

  SerializeMatrix("/home/daeyun/Dropbox/git/cs419_cpp/out/2.bin", hemi);

  return 0;
}
