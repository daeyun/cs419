//
// Created by daeyun on 4/11/17.
//

#include <chrono>
#include <memory>
#include <fstream>
#include <iostream>

#include "object.h"

#include "geom3d.h"

namespace render {

Sphere::Sphere(double r, const Vec3 &center)
    : center_(center) {
  set_r(r);
  set_bounding_box(BoundingBox(center_.array() - r, center_.array() + r));
}

bool Sphere::IntersectRay(const Ray &ray,
                          double *t,
                          Vec3 *hit_point,
                          Vec3 *normal) const {
  auto local_xyz = ray.origin() - center_;
  auto b = 2.0 * local_xyz.dot(ray.direction());

  double cq = local_xyz.squaredNorm() - r_sq_;
  auto disc = std::pow(b, 2) - 4.0 * cq;

  bool hit = disc > 0;

  if (hit) {
    auto e = std::sqrt(disc);
    auto t_front = -b - e;
    auto t_back = -b + e;
    auto t_closest =
        0.5 * ((std::abs(t_front) < std::abs(t_back)) ? t_front : t_back);

    *t = t_closest;

    *hit_point = ray.Point(t_closest);
    *normal = (*hit_point - center_).normalized();
  }
  return hit;
}

void Sphere::Translate(const Vec3 &xyz) {
  center_ += xyz;
}

bool Plane::IntersectRay(const Ray &ray,
                         double *t,
                         Vec3 *hit_point,
                         Vec3 *normal) const {
  auto denom = ray.direction().dot(oriented_point_.normal());
  bool hit = std::abs(denom) > 1e-7;

  if (hit) {
    *t = (oriented_point_.xyz() - ray.origin()).dot(oriented_point_.normal())
        / denom;
    *hit_point = ray.Point(*t);
    *normal = oriented_point_.normal();
  }

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
  ab_ = b - a;
  ac_ = c - a;

  auto normal = (ab_).cross(ac_);
  auto norm = normal.norm();
  area_ = norm * 0.5;
  normal_ = normal / norm;
}

bool Triangle::IsPointInTriangle(const Vec3 &point) const {
  auto ap = a_ - point;
  auto bp = b_ - point;
  auto cp = c_ - point;

  auto area_sum2 =
      ap.cross(bp).norm() + ap.cross(cp).norm() + bp.cross(cp).norm();
  return std::abs(area_sum2 - area_ * 2) < 1e-7;
}

// https://en.wikipedia.org/wiki/Moller–Trumbore_intersection_algorithm
bool Triangle::MollerTrumbore(const Ray &ray, double *out) const {
  auto P = ray.direction().cross(ac_);
  auto det = ab_.dot(P);
  if (std::abs(det) < kEpsilon) {
    return false;
  }
  auto inv_det = 1.0 / det;

  auto T = ray.origin() - a_;

  auto u = T.dot(P) * inv_det;
  if (u < 0.0 || u > 1.0) {
    return false;
  }

  auto Q = T.cross(ab_);

  auto v = ray.direction().dot(Q) * inv_det;
  if (v < 0.0 || u + v > 1.0) {
    return false;
  }

  auto t = ac_.dot(Q) * inv_det;

  if (t > kEpsilon) {
    *out = t;
    return true;
  }

  return false;
}

bool Triangle::IntersectRay(const Ray &ray,
                            double *t,
                            Vec3 *hit_point,
                            Vec3 *normal) const {

  // 5.8 seconds vs 7.2 seconds.

  bool hit = MollerTrumbore(ray, t);
  if (hit) {
    *hit_point = ray.Point(*t);
    *normal = normal_;
  }
  return hit;



//  double denom = ray.direction().dot(normal_);
//
//  // Otherwise parallel.
//  if (std::abs(denom) > 1e-7) {
//    double t_hit = (a_ - ray.origin()).dot(normal_) / denom;
//    auto p = ray.Point(t_hit);
//    if (IsPointInTriangle(p)) {
//      *t = t_hit;
//      *hit_point = p;
//      *normal = normal_;
//      return true;
//    }
//  }
//  return false;
}

void Triangle::Translate(const Vec3 &xyz) {
  a_ += xyz;
  b_ += xyz;
  c_ += xyz;
}
}
