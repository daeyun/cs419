//
// Created by daeyun on 4/12/17.
//

#ifndef CS419_CPP_RENDERER_H
#define CS419_CPP_RENDERER_H

#include <chrono>
#include <memory>
#include <fstream>
#include <ctime>
#include <iostream>
#include <opencv2/core/core.hpp>

#include "common.h"
#include "scene.h"
#include "sampling.h"
#include "benchmark.h"
#include "object.h"
#include "camera.h"
#include "world.h"

namespace render {

class Renderer {
 public:
  Renderer(std::shared_ptr<render::Viewport> viewport) : viewport_(viewport) {
    UseMultiJitteredSampling(3);
  }

  void UseMultiJitteredSampling(int grid_size = 3) {
    sampler_ = std::shared_ptr<render::MultiJittered>(new render::MultiJittered(viewport_, grid_size));
  }

  // `primary_ray_handler` is a function that computes the RGB values given an intersection point of a
  // primary ray. The renderer will take the average of those values to determine the final color of the pixel.
  void RenderScene(std::shared_ptr<World> world,
                   std::shared_ptr<Camera> camera,
                   std::function<void(const Scene &, const Intersection &, Vec3 *rgb_out)> primary_ray_handler,
                   cv::Mat *image) {
    *image = cv::Mat(cv::Size(viewport_->w(), viewport_->h()), CV_64FC4, cv::Scalar(0.0, 0.0, 0.0, 1.0));

    const Scene &scene = *world->scene();
#pragma omp parallel for
    for (int y = 0; y < viewport_->h(); ++y) {
      for (int x = 0; x < viewport_->w(); ++x) {
        auto &pixel = image->at<cv::Vec4d>(y, x);
        Points3d points;
        int hit_count = 0;

        sampler_->Sample(y, x, &points);

        Vec3 rgb;
        rgb.setZero();

        int num_points = static_cast<int>(points.cols());

        for (int i = 0; i < num_points; ++i) {
          Vec3 p = points.col(i);
          auto ray = Ray::FromCamera(p[0], p[1], *camera);
          render::Intersection intersection(ray);
          bool hit = scene.IntersectRay(ray, &intersection);
          if (hit) {
            Vec3 rgb_sample;
            primary_ray_handler(scene, intersection, &rgb_sample);
            rgb += rgb_sample;
            hit_count++;
          }
        }
        bool has_intersection = hit_count > 0;

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

    cv::Mat flipped;
    cv::flip(*image, flipped, 0);
    *image = flipped;
  }

 private:
  std::shared_ptr<render::Viewport> viewport_;
  std::shared_ptr<render::PixelSampler> sampler_;
};
}

#endif //CS419_CPP_RENDERER_H
