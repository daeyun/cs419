//
// Created by daeyun on 4/18/17.
//

#ifndef CS419_CPP_LIGHT_H_H
#define CS419_CPP_LIGHT_H_H

#include "common.h"

class Light {
 public:
  Light() {}
};

class PointLight : public Light {
 public:
  PointLight(const Vec3 &position) : position_(position) {}

  Vec3 Direction(const Vec3 &origin) {
    return (position_ - origin).normalized();
  }

  const Vec3 &position() const {
    return position_;
  }

 private:
  const Vec3 position_;
};

#endif //CS419_CPP_LIGHT_H_H
