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

  void set_scene(std::unique_ptr<Scene> scene) {
    scene_ = std::move(scene);
  }

  void set_camera(std::unique_ptr<Camera> camera) {
    camera_ = std::move(camera);
  }

  const Camera *camera() const {
    return camera_.get();
  }

  void AddLight(std::unique_ptr<PointLight> light) {
    lights_.push_back(std::move(light));
  }

  const Scene *scene() const {
    return scene_.get();
  }

  const std::vector<std::unique_ptr<PointLight>> &lights() const {
    return lights_;
  }

  const Vec3 &ambient() const {
    return ambient_;
  }

  void set_ambient(const Vec3 &ambient) {
    ambient_ = ambient;
  }

 private:
  std::unique_ptr<Scene> scene_;
  std::unique_ptr<Camera> camera_;
  std::vector<std::unique_ptr<PointLight>> lights_;
  Vec3 ambient_;
};
}

#endif //CS419_CPP_WORLD_H
