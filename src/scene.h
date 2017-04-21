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

class Scene {
 public:
  Scene() {}

  Scene(std::vector<std::unique_ptr<Object>> objects)
      : objects_(std::move(objects)) {}

  // `intersection` contains the input ray.
  bool IntersectRay(Intersection *intersection) const {
    const Ray *ray = intersection->ray();
    CHECK(ray != nullptr);

    double current_intersection;
    Vec3 current_hit, current_normal;

    double closest_intersection = kInfinity;
    Vec3 closest_hit, closest_normal;
    Object *closest_object = nullptr;

    for (const auto &object : objects_) {
      if (object->IntersectRay(*ray,
                               &current_intersection,
                               &current_hit,
                               &current_normal)) {
        if (current_intersection > 1e-6
            && current_intersection < closest_intersection) {
          closest_intersection = current_intersection;
          closest_hit = current_hit;
          closest_normal = current_normal;
          closest_object = object.get();
        }
      }
    }
    if (closest_object != nullptr) {
      intersection->set_t(closest_intersection);
      intersection->set_normal(closest_normal);
      intersection->set_hit_point(closest_hit);
      intersection->set_material(closest_object->material());
      return true;
    }
    return false;
  }

  void Translate(const Vec3 &xyz) {
    for (auto &object : objects_) {
      object->Translate(xyz);
    }
  }

  void AddObject(std::unique_ptr<Object> object) {
    objects_.push_back(std::move(object));
  }

  const Object *GetObject(int i) const {
    return objects_[i].get();
  }

  const std::vector<std::unique_ptr<Object>> &objects() const {
    return objects_;
  }

 private:
  std::vector<std::unique_ptr<Object>> objects_;
};
}

#endif //CS419_CPP_SCENE_H
