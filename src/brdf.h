//
// Created by daeyun on 4/16/17.
//

#ifndef CS419_CPP_BRDF_H
#define CS419_CPP_BRDF_H

#include <math.h>

#include "common.h"

class BRDF {
 public:
  BRDF() {}
};

class Lambertian : public BRDF {
 public:
  Lambertian(const Vec3 &color, double k) : color_(color), reflection_coefficient_(k) {
  }

  Vec3 Rho() const {
    return color_ * reflection_coefficient_;
  }

  Vec3 f() const {
    return Rho() * M_1_PI;
  }

 private:
  Vec3 color_;
  double reflection_coefficient_;  // k_d \in [0, 1]

};

#endif //CS419_CPP_BRDF_H
