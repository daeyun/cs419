//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_MATRIX_UTILS_H
#define CS419_CPP_MATRIX_UTILS_H

#include <cassert>
#include <Eigen/Dense>

#include "common.h"

namespace render {

Vec Seq(int stop);
Eigen::VectorXi SeqInt(int stop);

void MeshGridCoords(const Vec &x1, const Vec &x2, Points2d *points);

}

#endif //CS419_CPP_MATRIX_UTILS_H
