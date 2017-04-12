//
// Created by daeyun on 4/11/17.
//

#include "matrix_utils.h"

#include <Eigen/Dense>
#include "common.h"

namespace render {

Vec Seq(int stop) {
  return Vec::LinSpaced(stop, 0, stop - 1);
}

Eigen::VectorXi SeqInt(int stop) {
  return Eigen::VectorXi::LinSpaced(stop, 0, stop - 1);
}

void MeshGridCoords(const Vec &x1, const Vec &x2, Points2d *points) {
  auto n1 = x1.size();
  auto n2 = x2.size();

  points->resize(2, n1 * n2);
  points->row(0) = x1.replicate(n2, 1).transpose();

  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor> stacked(n1, n2);
  stacked.rowwise() = x2.transpose();
  points->row(1) = Eigen::Map<Eigen::VectorXd>(stacked.data(), stacked.size(), 1);
}
}
