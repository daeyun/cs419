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
  Renderer(const Viewport &viewport) : viewport_(viewport) {
    UseMultiJitteredSampling(3);
  }

  void UseMultiJitteredSampling(int grid_size = 3) {
    sampler_ = std::make_unique<render::MultiJittered>(viewport_, grid_size);
  }

  // `primary_ray_handler` is a function that computes the RGB values given an intersection point of a
  // primary ray. The renderer will take the average of those values to determine the final color of the pixel.
  void RenderScene(const World &world,
                   std::function<void(const World &,
                                      const Intersection &,
                                      Vec3 *rgb_out)> primary_ray_handler,
                   cv::Mat *image) {
    *image = cv::Mat(cv::Size(viewport_.w(), viewport_.h()),
                     CV_64FC4,
                     cv::Scalar(0.0, 0.0, 0.0, 1.0));

    const Scene *scene = world.scene();
    const Camera *camera = world.camera();
#pragma omp parallel for if(USE_OMP)
    for (int y = 0; y < viewport_.h(); ++y) {
      for (int x = 0; x < viewport_.w(); ++x) {
        auto &pixel = image->at<cv::Vec4d>(y, x);
        Points3d points;
        int hit_count = 0;

        sampler_->Sample(y, x, &points);

        Vec3 rgb;
        rgb.setZero();

        int num_points = static_cast<int>(points.cols());

        for (int i = 0; i < num_points; ++i) {
          Vec3 p = points.col(i);
          Intersection
              intersection(std::move(Ray::FromCamera(p[0], p[1], *camera)));
          bool hit = scene->IntersectRay(&intersection);
          if (hit) {
            auto normal = intersection.normal();
            // Render both sides. Flip any normals away from camera.
            if (normal.dot(camera->viewing_direction()) > 0.01) {
              intersection.set_normal(-normal);
            }

            Vec3 rgb_sample;
            primary_ray_handler(world, intersection, &rgb_sample);
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
  Viewport viewport_;
  std::unique_ptr<PixelSampler> sampler_;
};
}

#endif //CS419_CPP_RENDERER_H
