//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_SCENE_H
#define CS419_CPP_SCENE_H

#include <memory>
#include <iostream>

#include "object.h"
#include "common.h"

namespace render {

class Scene : public Object {
 public:
  Scene(const std::vector<std::shared_ptr<Object>> &objects) : objects_(objects) {
  }

  virtual bool IntersectRay(const std::shared_ptr<Ray> &ray, Intersection *intersection) const override {
    Intersection current_intersection(ray);
    Intersection closest_intersection(ray);
    closest_intersection.set_t(kInfinity);
    bool intersects = false;
    for (const auto &object : objects_) {
      if (object->IntersectRay(ray, &current_intersection)) {
        if (current_intersection.t() < closest_intersection.t()) {
          closest_intersection = current_intersection;
        }
        intersects = true;
      }
    }
    if (intersects) {
      *intersection = closest_intersection;
    }
    return intersects;
  }

  virtual void Translate(const Vec3 &xyz) override {
    for (auto &object : objects_) {
      object->Translate(xyz);
    }
  }

 private:
  std::vector<std::shared_ptr<Object>> objects_;
};
}

#endif //CS419_CPP_SCENE_H
