//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_GEOM3D_H
#define CS419_CPP_GEOM3D_H

#include <memory>
#include <opencv2/core/core.hpp>

#include "common.h"
#include "ray.h"

namespace render {

class OrientedPoint {
 public:
  OrientedPoint(const Vec3 &xyz, const Vec3 &normal) : xyz_(xyz), normal_(normal) {
    normal_.normalize();
  }

  void Translate(const Vec3 &xyz) {
    xyz_ = xyz_ + xyz;
  }

  const Vec3 &xyz() const {
    return xyz_;
  }

  void set_xyz(const Vec3 &xyz) {
    xyz_ = xyz;
  }

  const Vec3 &normal() const {
    return normal_;
  }

  void set_normal(const Vec3 &normal) {
    normal_ = normal;
  }

 private:
  Vec3 xyz_;
  Vec3 normal_;
};

class BoundingBox {
 public:
  BoundingBox(const Vec3 &corner1, const Vec3 &corner2);

  bool IsOverlapping(const BoundingBox &other) const;

  // `t_entry` is set to the entry displacement if the ray intersects the bounding box.
  // Returns true if `t_entry` was set.
  bool RayEntry(const Ray &ray, double *t_entry);

  const Vec3 &min() const {
    return min_;
  }

  void set_min(const Vec3 &min) {
    min_ = min;
  }

  const Vec3 &max() const {
    return max_;
  }

  void set_max(const Vec3 &max) {
    max_ = max;
  }

  const Vec3 &center() const {
    return center_;
  }

  void set_center(const Vec3 &center) {
    center_ = center;
  }

 private:
  Vec3 min_;
  Vec3 max_;
  Vec3 center_;
};

class Intersection {
 public:
  Intersection(std::shared_ptr<Ray> ray) : ray_(std::move(ray)) {}

  Intersection(std::shared_ptr<Ray> ray, double t, const Vec3 &normal) : ray_(std::move(ray)), t_(t), normal_(normal) {}

  std::shared_ptr<Ray> MirrorReflection() {
    if (mirror_reflected_ray_ == nullptr) {
      mirror_reflected_ray_ = std::move(ComputerMirrorReflection());
    }
    return mirror_reflected_ray_;
  }

  std::shared_ptr<Ray> ray() const {
    return ray_;
  }

  void set_ray(std::shared_ptr<Ray> ray) {
    ray_ = ray;
  }

  double t() const {
    return t_;
  }

  void set_t(double t) {
    t_ = t;
  }

  const Vec3 &normal() const {
    return normal_;
  }

  void set_normal(const Vec3 &normal) {
    normal_ = normal;
  }

 private:
  std::shared_ptr<Ray> ComputerMirrorReflection() {
    auto Ri = ray_->direction();
    auto Rr = Ri - 2 * normal_ * (Ri.dot(normal_));
    return std::make_shared<Ray>(ray_->Point(t_), Rr);
  }

  std::shared_ptr<Ray> ray_;
  std::shared_ptr<Ray> mirror_reflected_ray_ = nullptr;
  double t_;
  Vec3 normal_;
};

static void RotationAlignZ(const Vec3 &z_axis, Mat33 *rot) {
  if (std::abs(z_axis[0]) > std::abs(z_axis[1])) {
    rot->col(0).array() = Vec3(z_axis[2], 0, -z_axis[0]) / sqrt(z_axis[0] * z_axis[0] + z_axis[2] * z_axis[2]);
  } else {
    rot->col(0).array() = Vec3(0, -z_axis[2], z_axis[1]) / sqrt(z_axis[1] * z_axis[1] + z_axis[2] * z_axis[2]);
  }
  rot->col(1).array() = z_axis.cross(rot->col(0));
  rot->col(2).array() = z_axis;
}

}

#endif //CS419_CPP_GEOM3D_H
