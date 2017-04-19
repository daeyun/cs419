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
  World() {}

  void AddScene(const std::shared_ptr<Scene> &scene) {
    scene_ = scene;
  }

  void AddLight(const std::shared_ptr<Light> &light) {
    lights_.push_back(light);
  }

  const std::shared_ptr<Scene> &scene() const {
    return scene_;
  }

  const std::vector<std::shared_ptr<Light>> &lights() const {
    return lights_;
  }

 private:
  std::shared_ptr<Scene> scene_;
  std::vector<std::shared_ptr<Light>> lights_;
};
}

#endif //CS419_CPP_WORLD_H
