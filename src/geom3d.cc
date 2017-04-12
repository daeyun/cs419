//
// Created by daeyun on 4/11/17.
//

#include "geom3d.h"

#include "common.h"

namespace render {

BoundingBox::BoundingBox(const Vec3 &corner1, const Vec3 &corner2) {
  for (int i = 0; i < 3; ++i) {
    if (corner1[i] < corner2[i]) {
      min_[i] = corner1[i];
      max_[i] = corner2[i];
    } else {
      min_[i] = corner2[i];
      max_[i] = corner1[i];
    }
  }

  center_ = (corner1 + corner2) * 0.5;
}

bool BoundingBox::IsOverlapping(const BoundingBox &other) const {
  const auto &other_min = other.min();
  const auto &other_max = other.max();
  return (max_.array() > other_min.array()).all() && (min_.array() < other_max.array()).all();
}

// `t_entry` is set to the entry displacement if the ray intersects the bounding box.
// Returns true if `t_entry` was set.
bool BoundingBox::RayEntry(const Ray &ray, double *t_entry) {
  auto dmin = (min_ - ray.origin()).array() / ray.direction().array();
  auto dmax = (max_ - ray.origin()).array() / ray.direction().array();

  double entry = dmin.minCoeff();
  double exit = dmax.minCoeff();

  if (entry < exit) {
    *t_entry = entry;
    return true;
  }
  return false;
}
}

