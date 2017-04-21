//
// Created by daeyun on 4/16/17.
//

#include "brdf.h"

namespace render {

Lambertian::Lambertian(const Vec3 &color)
    : color_(color), reflection_coefficient_(1.0) {
  reflectance_ = color_ * reflection_coefficient_;
}

Lambertian::Lambertian(const Vec3 &color, double reflection_coeff)
    : color_(color), reflection_coefficient_(reflection_coeff) {
  reflectance_ = color_ * reflection_coefficient_;
}

Vec3 Lambertian::Reflectance(const Intersection &hit_point) const {
  return reflectance_;
}

Vec3 Lambertian::BRDFValue(const Intersection &hit_point,
                           const Vec3 &wi,
                           const Vec3 &wo) const {
  return Reflectance(hit_point) * M_1_PI;
}

void Lambertian::SampleRayDirection(const Intersection &hit_point,
                                    const Vec3 &v,
                                    Vec3 *sampled,
                                    double *pdf) const {
  sampler()->Sample(hit_point.normal(), sampled, pdf);
}

void Lambertian::SampleRayDirection(const Intersection &hit_point,
                                    const Vec3 &v,
                                    int num,
                                    Points3d *sampled,
                                    Vec *pdf) const {
  sampler()->SampleMany(hit_point.normal(), num, sampled, pdf);
}

}
