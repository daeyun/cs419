//
// Created by daeyun on 4/11/17.
//

#include "viewing.h"
#include "matrix_utils.h"

namespace render {

Viewport::Viewport(int h, int w) : h_(h), w_(w) {
  min_corner_ = {-1.0, -1.0, 0.0};
  max_corner_ = {1.0, 1.0, 0.0};
  s_ = 2.0 / w_;
}

void Viewport::GetPixelCenter(int row, int column, Vec3 *pixel_center) {
  (*pixel_center)[0] = s_ * (column - (w_ * 0.5) + 0.5);
  (*pixel_center)[1] = s_ * (row - (h_ * 0.5) + 0.5);
  (*pixel_center)[2] = min_corner_[2];
}

void Viewport::GetPixelCorner(int row, int column, Vec3 *pixel_corner) {
  (*pixel_corner)[0] = s_ * (column - (w_ * 0.5));
  (*pixel_corner)[1] = s_ * (row - (h_ * 0.5));
  (*pixel_corner)[2] = min_corner_[2];
}

void Viewport::GetPixelCorners(Points2d *points) {
  auto xs = Seq(w_);
  auto ys = Seq(h_);

  MeshGridCoords(xs, ys, points);

  Vec2 wh(w_, h_);
  points->colwise() -= wh * 0.5;
  points->array() *= s_;
}

void Viewport::GetPixelIndices(Points2i *indices) {
  auto x1 = SeqInt(h_);
  auto x2 = SeqInt(w_);

  auto n1 = x1.size();
  auto n2 = x2.size();

  indices->resize(2, n1 * n2);
  indices->row(0) = x1.replicate(n2, 1).transpose();

  Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> stacked(n1, n2);
  stacked.rowwise() = x2.transpose();
  indices->row(1) = Eigen::Map<Eigen::VectorXi>(stacked.data(), stacked.size(), 1);
}

void Viewport::GetPixelCenters(Points2d *points) {
  GetPixelCorners(points);
  points->array() += s_ * 0.5;
}
}
