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
#include "src/sampling.h"
#include "src/viewing.h"
#include "src/scene.h"

using namespace render;

class Renderer {
 public:
  Renderer(std::shared_ptr<Viewport> viewport) : viewport_(viewport) {
    UseMultiJitteredSampling(3);
  }

  void UseMultiJitteredSampling(int grid_size = 3) {
    sampler_ = std::shared_ptr<MultiJittered>(new MultiJittered(viewport_, 3));
  }

  void RenderScene(std::shared_ptr<Scene> scene,
                   std::function<void(const Intersection &, Vec3 *)> intersection_handler,
                   cv::Mat *image) {
    *image = cv::Mat(cv::Size(viewport_->w(), viewport_->h()), CV_64FC4, cv::Scalar(0.0, 0.0, 0.0, 1.0));

    Timer timer1;
    Timer timer2;
    Timer timer3;

    for (int y = 0; y < viewport_->h(); ++y) {
      for (int x = 0; x < viewport_->w(); ++x) {
        auto &pixel = image->at<cv::Vec4d>(y, x);
        Points3d points;
        int hit_count = 0;

        timer1.Tic();
        sampler_->Sample(y, x, &points);
        timer1.Toc();

        Vec3 rgb;
        rgb.setZero();

        int num_points = static_cast<int>(points.cols());
        int hit_count_threshold = static_cast<int>(num_points * 0.5) + 1;
        bool has_intersection = true;

        for (int i = 0; i < num_points; ++i) {
          Vec3 direction(0, 0, 1);
          Vec3 p = points.col(i);
          p[2] = -1;
          auto ray = std::make_shared<Ray>(p, direction);
          Intersection intersection(ray);
          timer2.Tic();
          bool hit = scene->IntersectRay(ray, &intersection);
          timer2.Toc();
          if (hit) {
            timer3.Tic();
            Vec3 rgb_sample;
            intersection_handler(intersection, &rgb_sample);
            rgb += rgb_sample;
            hit_count++;
            timer3.Toc();
          }

          if (num_points - i + hit_count <= hit_count_threshold) {
            has_intersection = false;
            break;
          }
        }

        if (has_intersection) {
          rgb /= static_cast<double>(hit_count);
          pixel[0] = rgb[0];
          pixel[1] = rgb[1];
          pixel[2] = rgb[2];
        } else {
          pixel[3] = 0;
        }
      }
    }
    std::cout << "sampling: " << timer1.TotalElapsedMilliSeconds() << std::endl;
    std::cout << "intersection: " << timer2.TotalElapsedMilliSeconds() << std::endl;
    std::cout << "pixelization: " << timer3.TotalElapsedMilliSeconds() << std::endl;
  }

 private:
  std::shared_ptr<Viewport> viewport_;
  std::shared_ptr<Sampler> sampler_;
};

int main() {
  auto start = MilliSecondsSinceEpoch();

  auto viewport = std::make_shared<Viewport>(512, 512);

//  Sphere sphere(0.8, {0, 0, 0});

  std::shared_ptr<Scene> scene(new Scene(
      {
          std::shared_ptr<Triangle>(new Triangle({-0.7, -0.7, 0.4}, {1, 0, -0.8}, {0, 1, -0.8})),
          std::shared_ptr<Sphere>(new Sphere(0.8, {0.5, 0.5, 0.0})),
      }
  ));

  auto depth_rgb = [](const Intersection &intersection, Vec3 *rgb) {
    rgb->setZero();
    (*rgb)[0] = intersection.t();
  };
  auto normal_rgb = [](const Intersection &intersection, Vec3 *rgb) {
    rgb->setZero();
    *rgb = intersection.normal();
  };

  cv::Mat image;
//  Render(scene, viewport, depth_rgb, &image);
  Renderer renderer(viewport);
  renderer.UseMultiJitteredSampling(3);
  renderer.RenderScene(scene, normal_rgb, &image);

  std::string outfile = "/home/daeyun/Dropbox/git/cs419_cpp/out/1.png";
  SavePng(outfile, image);

  std::string outfile2 = "/home/daeyun/Dropbox/git/cs419_cpp/out/1.bin";
  SerializeRgba(outfile2, image);

  auto end = MilliSecondsSinceEpoch();

  std::cout << "Time elapsed: " << end - start << " ms" << std::endl;

  return 0;
}
