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
  Viewport viewport(512, 512);

  auto depth_rgb =
      [](const World &world,
         const Intersection &intersection,
         Vec3 *rgb) {
        rgb->setZero();
        (*rgb)[0] = intersection.t();
      };
  auto normal_rgb =
      [](const World &world,
         const Intersection &intersection,
         Vec3 *rgb) {
        rgb->setZero();
        *rgb = intersection.normal();
      };

  std::function<void(const World &,
                     const Intersection &,
                     Vec3 *)>
      rt_rgb = [&rt_rgb](const World &world,
                         const Intersection &intersection,
                         Vec3 *rgb) {
    rgb->setZero();

    const Vec3 &normal = intersection.normal();
    const auto point = intersection.ray()->Point(intersection.t());

    const Scene *scene = world.scene();
    for (const auto &light: world.lights()) {
      auto direction = light->Direction(point);
      if (normal.dot(direction) < 0) {
        continue;
      }
      auto distance_to_light = (light->position() - point).norm();
      Intersection light_intersection(std::make_unique<Ray>(point, direction));
      bool hit =
          scene->IntersectRay(&light_intersection);

      if (!hit) {
        *rgb += Vec3(0.4, 0.4, 0.4) * normal.dot(direction);
      }
    }

    Vec3 L;
    L.setZero();

    CHECK(intersection.material() != nullptr);

    const int n = 10;
    const auto &brdf = intersection.material()->brdf;

    Points3d samples;
    Vec pdfs;

    brdf->SampleRayDirection(intersection,
                             intersection.ray()->direction(),
                             n,
                             &samples,
                             &pdfs);

    for (int i = 0; i < n; ++i) {
      auto sampled = samples.col(i);
      auto pdf = pdfs(i);

      Intersection light_intersection(std::make_unique<Ray>(point, sampled));
      bool hit =
          scene->IntersectRay(&light_intersection);

      // This is a constant rho/pi for lambertian BRDF.
      auto f = brdf->BRDFValue(
          intersection, sampled, -intersection.ray()->direction());

      // Incident radiance from the ambient light.
      auto Li_ambiant = world.ambient();

      if (!hit) {
        L.array() +=
            f.array() * Li_ambiant.array() * (normal.dot(sampled) / pdf);
      }
    }
    L /= static_cast<double>(n);
    *rgb += L;

  };

//  std::unique_ptr<Scene> scene(new Scene(
//      {
//          std::unique_ptr<Triangle>(new Triangle({-0.7, -0.7, 0.4}, {1, 0, -0.8}, {0, 1, -0.8})),
//          std::unique_ptr<Sphere>(new Sphere(0.8, {0.5, 0.5, 0.0})),
//      }
//  ));
  auto world = std::make_unique<World>();
  auto scene = std::make_unique<Scene>();

  scene->AddObject(
      std::make_unique<Triangle>(Vec3{1.5, 1.5, 0},
                                 Vec3{1.5, -1.5, 0},
                                 Vec3{-1.5, 1.5, 0}));

  scene->AddObject(
      std::make_unique<Triangle>(Vec3{-1.5, -1.5, 0},
                                 Vec3{1.5, -1.5, 0},
                                 Vec3{-1.5, 1.5, 0}));

  scene->AddObject(std::make_unique<Triangle>(Vec3{0, 0, 1},
                                              Vec3{1, 0, 0},
                                              Vec3{0, 0, 0}));

  scene->AddObject(std::make_unique<Sphere>(0.5, Vec3{0.0, 0.0, 0.5}));

  scene->AddObject(std::make_unique<Sphere>(0.3, Vec3{1.0, 0.0, 0.3}));

  scene->AddObject(std::make_unique<Sphere>(0.3, Vec3{0.0, 1.0, 0.3}));

  scene->AddObject(std::make_unique<Triangle>(Vec3{-1.3, 1.0, 0},
                                              Vec3{-1.3, -1.0, 0},
                                              Vec3{-1.3, 1.0, 0.5}));

  scene->AddObject(std::make_unique<Triangle>(Vec3{-1, 0.8, 0},
                                              Vec3{-1, -0.8, 0},
                                              Vec3{-1, 0.8, 0.5}));

  world->set_scene(std::move(scene));

  auto mat = std::make_unique<Material>();
  mat->brdf = std::make_unique<Lambertian>(Vec3{0.5, 0.5, 0.5});
  mat->brdf->set_sampler(std::make_unique<HemisphereSampler>(1.0));

  for (auto &obj: world->scene()->objects()) {
    obj->set_material(mat.get());
  }


//  std::unique_ptr<Scene> scene(new Scene(
//      {
//          std::unique_ptr<Sphere>(new Sphere(0.5, {0.0, 0.0, 0.0})),
//      }
//  ));

  constexpr int kNumRepeats = 1;

  std::vector<long> times;

  cv::Mat image;

  FrustumParams frustum;
  frustum.near = 3.0;
  frustum.far = 10.0;

  world->set_camera(std::make_unique<PerspectiveCamera>(
      Vec3{3, 3, 3}, Vec3{0, 0, 0}, Vec3{0, 0, 1}, frustum));

  world->set_ambient({0.6, 0.6, 0.6});

  world->AddLight(std::make_unique<PointLight>(Vec3{0, 1, 0.9}));

  for (int j = 0; j < kNumRepeats; ++j) {
    auto start = MilliSecondsSinceEpoch();

    cv::Mat img;
    Renderer renderer(viewport);
    renderer.UseMultiJitteredSampling(8);

    renderer.RenderScene(*world, rt_rgb, &img);

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

//  auto start = MilliSecondsSinceEpoch();
//  auto sampler = HemisphereSampler(1.0);
//  double pdf;
//  Points3d hemi(3, 10000);
//  for (int i = 0; i < 10000; ++i) {
//    Vec3 sampled;
//    sampler.Sample({-1, -1, -1}, &sampled, &pdf);
//    hemi.col(i).array() = sampled;
//  }
//  auto end = MilliSecondsSinceEpoch();
//  std::cout << "Time elapsed: " << end - start << " ms" << std::endl;
//
//  SerializeMatrix("/home/daeyun/Dropbox/git/cs419_cpp/out/2.bin", hemi);

  return 0;
}
