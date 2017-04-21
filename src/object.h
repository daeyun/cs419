//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_OBJECT_H
#define CS419_CPP_OBJECT_H

#include "common.h"
#include "ray.h"
#include "geom3d.h"
#include "brdf.h"

namespace render {

class BRDF;

class Intersection;

struct Material {
  std::unique_ptr<BRDF> brdf;
};

class Object {
 public:
  Object() {}

  virtual bool IntersectRay(const Ray &ray,
                            double *t,
                            Vec3 *hit_point,
                            Vec3 *normal) const = 0;
  virtual void Translate(const Vec3 &xyz) = 0;

  const BoundingBox &bounding_box() const {
    return bounding_box_;
  }

  void set_bounding_box(const BoundingBox &box) {
    bounding_box_ = box;
  }

  const Material *material() const {
    return material_;
  }

  void set_material(const Material *material) {
    material_ = material;
  }

 private:
  BoundingBox bounding_box_;
  const Material *material_ = nullptr;
};

class Sphere : public Object {
 public:
  Sphere(double r, const Vec3 &center);

  virtual bool IntersectRay(const Ray &ray,
                            double *t,
                            Vec3 *hit_point,
                            Vec3 *normal) const override;
  virtual void Translate(const Vec3 &xyz) override;

  void set_r(double r) {
    r_ = r;
    r_sq_ = std::pow(r_, 2);
  }

 private:
  double r_;
  double r_sq_;
  Vec3 center_;
};

class Plane : public Object {
 public:
  Plane(const OrientedPoint &point) : oriented_point_(point) {}

  virtual bool IntersectRay(const Ray &ray,
                            double *t,
                            Vec3 *hit_point,
                            Vec3 *normal) const override;

  const OrientedPoint &oriented_point() const {
    return oriented_point_;
  }

  void set_oriented_point(const OrientedPoint &oriented_point) {
    oriented_point_ = oriented_point;
  }

  virtual void Translate(const Vec3 &xyz) override;

 private:
  OrientedPoint oriented_point_;
};

class Triangle : public Object {
 public:
  Triangle(const Vec3 &a, const Vec3 &b, const Vec3 &c);

  virtual bool IntersectRay(const Ray &ray,
                            double *t,
                            Vec3 *hit_point,
                            Vec3 *normal) const override;
  virtual void Translate(const Vec3 &xyz) override;

  void SetVertices(const Vec3 &a, const Vec3 &b, const Vec3 &c);

  bool IsPointInTriangle(const Vec3 &point) const;

  bool MollerTrumbore(const Ray &ray, double *out) const;

  const Vec3 &a() const {
    return a_;
  }

  const Vec3 &b() const {
    return b_;
  }

  const Vec3 &c() const {
    return c_;
  }

  double area() const {
    return area_;
  }

  const Vec3 &normal() const {
    return normal_;
  }

 private:
  Vec3 a_;
  Vec3 b_;
  Vec3 c_;
  Vec3 normal_;
  Vec3 ab_;
  Vec3 ac_;

  double area_;
};

class Intersection {
 public:
  Intersection(std::unique_ptr<Ray> ray) : ray_(std::move(ray)) {}

  Intersection(std::unique_ptr<Ray> ray, double t, const Vec3 &normal) : ray_(
      std::move(ray)), t_(t), normal_(normal) {}

  Intersection(std::unique_ptr<Ray> ray,
               double t,
               const Vec3 &normal,
               const Material *material) :
      ray_(std::move(ray)),
      t_(t),
      normal_(normal),
      material_(material) {}

  const Ray *MirrorReflection() {
    if (mirror_reflected_ray_ == nullptr) {
      mirror_reflected_ray_ = std::move(ComputerMirrorReflection());
    }
    return mirror_reflected_ray_.get();
  }

  const Ray *ray() const {
    return ray_.get();
  }

  void set_ray(std::unique_ptr<Ray> ray) {
    ray_ = std::move(ray);
  }

  double t() const {
    return t_;
  }

  void set_t(double t) {
    t_ = t;
  }

  const Vec3 &normal() const {
    return normal_;
  }

  void set_normal(const Vec3 &normal) {
    normal_ = normal;
  }

  const Material *material() const {
    return material_;
  }

  void set_material(const Material *mat) {
    material_ = mat;
  }

  const Vec3 &hit_point() const {
    return hit_point_;
  }

  void set_hit_point(const Vec3 &hit_point) {
    hit_point_ = hit_point;
  }

 private:
  std::unique_ptr<Ray> ComputerMirrorReflection() {
    auto Ri = ray_->direction();
    auto Rr = Ri - 2 * normal_ * (Ri.dot(normal_));
    return std::make_unique<Ray>(ray_->Point(t_), Rr);
  }

  std::unique_ptr<Ray> ray_;
  std::unique_ptr<Ray> mirror_reflected_ray_ = nullptr;
  const Material *material_;
  double t_;
  Vec3 normal_;
  Vec3 hit_point_;
};
}

#endif //CS419_CPP_OBJECT_H
