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

 private:
  Vec3 position_;
};

#endif //CS419_CPP_LIGHT_H_H
