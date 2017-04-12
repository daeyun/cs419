//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_RAY_H
#define CS419_CPP_RAY_H

#include "common.h"

namespace render {

class Ray {
 public:
  Ray(const Vec3 &origin, const Vec3 &direction);

  Vec3 Point(double t) const;

  const Vec3 &origin() const {
    return origin_;
  }

  void set_origin(const Vec3 &origin) {
    origin_ = origin;
  }

  const Vec3 &direction() const {
    return direction_;
  }

  void set_direction(const Vec3 &direction) {
    direction_ = direction;
  }

 private:
  Vec3 origin_;
  Vec3 direction_;
};
}

#endif //CS419_CPP_RAY_H
