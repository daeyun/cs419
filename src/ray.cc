//
// Created by daeyun on 4/11/17.
//

#include "ray.h"

namespace render {

Ray::Ray(const Vec3 &origin, const Vec3 &direction) : origin_(origin), direction_(direction) {
  direction_.normalize();
}

Vec3 Ray::Point(double t) const {
  return origin() + t * direction();
}
}

