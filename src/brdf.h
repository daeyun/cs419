//
// Created by daeyun on 4/16/17.
//

#ifndef CS419_CPP_BRDF_H
#define CS419_CPP_BRDF_H

#include <math.h>

#include "common.h"
#include "object.h"
#include "sampling.h"

namespace render {

class Intersection;

class BRDF {
 public:
  BRDF() {}

  // The ratio of reflected flux to incident flux in the hemisphere above p.
  // Denoted \rho. Satisfies \rho < 1.
  virtual Vec3 Reflectance(const Intersection &hit_point) const = 0;

  // The ratio of reflected radiance (power per unit solid angle) to irradiance
  // (power per unit surface area) incident on the surface. i.e. Light reflected
  // from wi to wo. Denoted f_r
  virtual Vec3 BRDFValue(const Intersection &hit_point,
                         const Vec3 &wi,
                         const Vec3 &wo) const = 0;

  // Randomly samples a point on the unit hemisphere above p. Used for computing
  // wi from wo.
  virtual void SampleRayDirection(const Intersection &hit_point,
                                  const Vec3 &v,
                                  Vec3 *sampled,
                                  double *pdf) const = 0;

  virtual void SampleRayDirection(const Intersection &hit_point,
                                  const Vec3 &v,
                                  int num,
                                  Points3d *sampled,
                                  Vec *pdf) const = 0;

  const HemisphereSampler *sampler() const {
    return sampler_.get();
  }

  void set_sampler(std::unique_ptr<HemisphereSampler> sampler) {
    sampler_ = std::move(sampler);
  }

 private:
  std::unique_ptr<HemisphereSampler> sampler_;
};

class Lambertian : public BRDF {
 public:
  Lambertian(const Vec3 &color);

  Lambertian(const Vec3 &color, double reflection_coeff);

  virtual Vec3 Reflectance(const Intersection &hit_point) const override;

  virtual Vec3 BRDFValue(const Intersection &hit_point,
                         const Vec3 &wi,
                         const Vec3 &wo) const override;

  virtual void SampleRayDirection(const Intersection &hit_point,
                                  const Vec3 &v,
                                  Vec3 *sampled, double *pdf) const override;

  virtual void SampleRayDirection(const Intersection &hit_point,
                                  const Vec3 &v,
                                  int num,
                                  Points3d *sampled, Vec *pdf) const override;

  const Vec3 &color() const {
    return color_;
  }

  double reflection_coefficient() const {
    return reflection_coefficient_;
  }

  void set_color(const Vec3 &color) {
    color_ = color;
    reflectance_ = color_ * reflection_coefficient_;
  }

  void set_reflection_coefficient(double k) {
    reflection_coefficient_ = k;
    reflectance_ = color_ * reflection_coefficient_;
  }

 private:
  Vec3 color_;
  double reflection_coefficient_;  // k_d \in [0, 1]
  Vec3 reflectance_;

};

}

#endif //CS419_CPP_BRDF_H
