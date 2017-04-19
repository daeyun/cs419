//
// Created by daeyun on 4/11/17.
//

#include <chrono>
#include <memory>
#include <fstream>
#include <iostream>

#include "object.h"

namespace render {

Sphere::Sphere(double r, const Vec3 &center)
    : center_(center) {
  set_r(r);
  auto box = std::make_shared<BoundingBox>(center_.array() - r, center_.array() + r);
  set_bounding_box(box);
}

bool Sphere::IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const {
  auto local_xyz = ray->origin() - center_;
  auto b = 2.0 * local_xyz.dot(ray->direction());

  auto cq = local_xyz.squaredNorm() - r_sq_;
  auto disc = std::pow(b, 2) - 4.0 * cq;

  bool hit = disc > 0;

  auto e = std::sqrt(disc);
  auto t_front = -b - e;
  auto t_back = -b + e;
  auto t = 0.5 * ((std::abs(t_front) < std::abs(t_back)) ? t_front : t_back);

  intersection->set_t(t);
  intersection->set_ray(ray);

  auto hit_point = ray->Point(t);
  intersection->set_normal((hit_point - center_).normalized());

  return hit;
}

void Sphere::Translate(const Vec3 &xyz) {
  center_ += xyz;
}

bool Plane::IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const {
  auto denom = ray->direction().dot(oriented_point_.normal());
  bool hit = std::abs(denom) > 1e-7;

  double t = (oriented_point_.xyz() - ray->origin()).dot(oriented_point_.normal()) / denom;

  intersection->set_t(t);
  intersection->set_ray(ray);
  intersection->set_normal(oriented_point_.normal());

  return hit;
}

void Plane::Translate(const Vec3 &xyz) {
  oriented_point_.Translate(xyz);
}

Triangle::Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c) {
  SetVertices(a, b, c);
}

void Triangle::SetVertices(const Vec3 &a, const Vec3 &b, const Vec3 &c) {
  a_ = a;
  b_ = b;
  c_ = c;

  auto normal = (b - a).cross(c - a);
  auto norm = normal.norm();
  area2_ = norm;
  area_ = norm * 0.5;
  normal_ = normal / norm;
}

bool Triangle::IsPointInTriangle(const Vec3 &point) const {
  auto ap = a_ - point;
  auto bp = b_ - point;
  auto cp = c_ - point;

  auto area_sum2 = ap.cross(bp).norm() + ap.cross(cp).norm() + bp.cross(cp).norm();
  return std::abs(area_sum2 - area2_) < 1e-7;
}

bool Triangle::IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const {
  double denom = ray->direction().dot(normal_);
  if (std::abs(denom) < 1e-7) {
    // Parallel.
    return false;
  }

  double t = (a_ - ray->origin()).dot(normal_) / denom;
  if (!IsPointInTriangle(ray->Point(t))) {
    return false;
  }

  intersection->set_t(t);
  intersection->set_ray(ray);
  intersection->set_normal(normal_);

  return true;
}

void Triangle::Translate(const Vec3 &xyz) {
  a_ += xyz;
  b_ += xyz;
  c_ += xyz;
}

}
