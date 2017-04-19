//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_OBJECT_H
#define CS419_CPP_OBJECT_H

#include "common.h"
#include "ray.h"
#include "geom3d.h"

namespace render {

struct Material {
  Vec3 rgb;
};

class Object {
 public:
  Object() {}

  ~Object() = default;

  virtual bool IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const = 0;
  virtual void Translate(const Vec3 &xyz) = 0;

  std::shared_ptr<BoundingBox> bounding_box() {
    return bounding_box_;
  }

  void set_bounding_box(std::shared_ptr<BoundingBox> box) {
    bounding_box_ = box;
  }

  std::shared_ptr<Material> material() const {
    return material_;
  }

  void set_material(std::shared_ptr<Material> material) {
    material_ = material;
  }

 private:
  std::shared_ptr<BoundingBox> bounding_box_;
  std::shared_ptr<Material> material_;
};

class Sphere : public Object {
 public:
  Sphere(double r, const Vec3 &center);

  virtual bool IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const override;
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

  virtual bool IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const override;

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

  virtual bool IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const override;
  virtual void Translate(const Vec3 &xyz) override;

  void SetVertices(const Vec3 &a, const Vec3 &b, const Vec3 &c);

  bool IsPointInTriangle(const Vec3 &point) const;

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

  // area times 2
  double area_;
  double area2_;
};
}

#endif //CS419_CPP_OBJECT_H
