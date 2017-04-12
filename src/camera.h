//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_CAMERA_H
#define CS419_CPP_CAMERA_H

#include "common.h"

class Camera {
 public:
  Camera() {}

  Camera(const Vec3 &camera_position, const Vec3 &lookat_position, const Vec3 &up)
      : position_(camera_position), lookat_position_(lookat_position), up_(up) {
    auto viewing_direction = (lookat_position - camera_position).normalized();
    auto right = viewing_direction.cross(up).normalized();
    auto up_vector = right.cross(viewing_direction);

    world_to_cam_(0, 0) = right[0];
    world_to_cam_(0, 1) = right[1];
    world_to_cam_(0, 2) = right[2];
    world_to_cam_(1, 0) = up_vector[0];
    world_to_cam_(1, 1) = up_vector[1];
    world_to_cam_(1, 2) = up_vector[2];
    world_to_cam_(2, 0) = -viewing_direction[0];
    world_to_cam_(2, 1) = -viewing_direction[1];
    world_to_cam_(2, 2) = -viewing_direction[2];

//    Vec3 translation = -(world_to_cam_.get_minor<3, 3>(0, 0) * camera_position);
//    world_to_cam_(0, 3) = translation[0];
//    world_to_cam_(1, 3) = translation[1];
//    world_to_cam_(2, 3) = translation[2];

//    cv::Mat(world_to_cam_, false).colRange(0, 3).t();

  }

  void WorldToCam(const Vec3 &xyz, Vec3 *out) {
    auto hom = cv::Matx<double, 4, 1>(xyz[0], xyz[1], xyz[2], 0.0);
//    cv::Mat ret = cv::Mat(world_to_cam_, false) * cv::Mat(hom, false);
//    ret.copyTo(*out);
  }

 private:
  Vec3 position_;
  Vec3 lookat_position_;
  Vec3 up_;
  Mat34 world_to_cam_;
  Mat34 cam_to_world_;
};

#endif //CS419_CPP_CAMERA_H
