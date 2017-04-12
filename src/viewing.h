//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_VIEWING_H
#define CS419_CPP_VIEWING_H

#include "common.h"
#include "geom3d.h"

namespace render {

class Viewport {
 public:
  Viewport(int h, int w);

  void GetPixelCenter(int row, int column, Vec3 *pixel_center);
  void GetPixelCorner(int row, int column, Vec3 *pixel_corner);
  void GetPixelCenters(Points2d *points);
  void GetPixelCorners(Points2d *points);
  void GetPixelIndices(Points2i *indices);

  int h() const {
    return h_;
  }

  int w() const {
    return w_;
  }

  double s() const {
    return s_;
  }

  const Vec3 &min_corner() const {
    return min_corner_;
  }

  const Vec3 &max_corner() const {
    return max_corner_;
  }

 private:
  int h_;
  int w_;
  double s_;
  Vec3 min_corner_;
  Vec3 max_corner_;
};
}

#endif //CS419_CPP_VIEWING_H
