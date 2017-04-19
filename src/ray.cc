//
// Created by daeyun on 4/11/17.
//

#include "ray.h"

namespace render {

Ray::Ray(const Vec3 &origin, const Vec3 &direction) : origin_(origin), direction_(direction) {
  direction_.normalize();
}

std::shared_ptr<Ray> Ray::FromCamera(double x, double y, const Camera &camera) {
  Vec3 p(x, y, -1);
  Vec3 p_cam;
  camera.FrustumToCam(p, &p_cam);
  camera.CamToWorld(p_cam, &p_cam);

  if (dynamic_cast<const PerspectiveCamera *>(&camera)) {
    Vec3 d = (p_cam - camera.position()).normalized();
    return std::make_shared<Ray>(p_cam, d);
  } else {
    auto d = camera.lookat_position() - camera.position();
    return std::make_shared<Ray>(p_cam, d);
  }
}

Vec3 Ray::Point(double t) const {
  return origin() + t * direction();
}
}

