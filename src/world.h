//
// Created by daeyun on 4/18/17.
//

#ifndef CS419_CPP_WORLD_H
#define CS419_CPP_WORLD_H

#include <memory>
#include "scene.h"
#include "light.h"

namespace render {
class World {
 public:
  World() : ambient_(0.05, 0.05, 0.05) {}

  void set_scene(const std::shared_ptr<Scene> &scene) {
    scene_ = scene;
  }

  void set_camera(const std::shared_ptr<Camera> &camera) {
    camera_ = camera;
  }

  const std::shared_ptr<Camera> &camera() const {
    return camera_;
  }

  void AddLight(const std::shared_ptr<PointLight> &light) {
    lights_.push_back(light);
  }

  const std::shared_ptr<Scene> &scene() const {
    return scene_;
  }

  const std::vector<std::shared_ptr<PointLight>> &lights() const {
    return lights_;
  }

  const Vec3 &ambient() const {
    return ambient_;
  }

  void set_ambient(const Vec3 &ambient) {
    ambient_ = ambient;
  }

 private:
  std::shared_ptr<Scene> scene_;
  std::shared_ptr<Camera> camera_;
  std::vector<std::shared_ptr<PointLight>> lights_;
  Vec3 ambient_;
};
}

#endif //CS419_CPP_WORLD_H
