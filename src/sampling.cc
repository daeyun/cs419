//
// Created by daeyun on 4/11/17.
//

#include "sampling.h"
#include "viewing.h"
#include "matrix_utils.h"
#include "benchmark.h"

#include <chrono>
#include <fstream>

namespace render {
MultiJittered::MultiJittered(const std::shared_ptr<Viewport> &viewport, int grid_size)
    : PixelSampler(viewport), grid_size_(grid_size), distribution_(0, default_pool_size_ - 1) {
  for (int i = 0; i < grid_size * grid_size; ++i) {
    rook_x_.push_back(static_cast<double>(i));
  }
  num_samples_per_pixel_ = grid_size_ * grid_size_;
  SetRookY();

  GenerateSamplePool(default_pool_size_);
}

// Shuffles the x, y indices of a grid in a way that satisfies the rook condition.
void MultiJittered::ShuffleRookCoords() {
#pragma omp critical
  {
    // Shuffle the x indices in-place.
    auto it = rook_x_.begin();
    auto it_end = it + grid_size_;
    for (int i = 0; i < grid_size_; ++i) {
      random_shuffle(it, it_end);
      it += grid_size_;
      it_end += grid_size_;
    }

    // Takes the transpose of x coordinate array to derive the y coordinates.
    // Same as shuffling each column separately.
    SetRookY();

    // Shuffle x again.
    it = rook_x_.begin();
    it_end = it + grid_size_;
    for (int i = 0; i < grid_size_; ++i) {
      random_shuffle(it, it_end);
      it += grid_size_;
      it_end += grid_size_;
    }
  }
}

// Generates 3D points that correspond to a pixel at (y, x).
void MultiJittered::Sample(int row, int col, Points3d *points) {
  if (pool_.size() > 0) {
    Vec3 pixel_corner;
    pixel_corner.setZero();
    viewport()->GetPixelCorner(row, col, &pixel_corner);

    auto i = distribution_(rand_);
    *points = pool_[i].colwise() + pixel_corner;
  } else {
    ShuffleRookCoords();

    Points3d pts(3, num_samples_per_pixel_);

    Vec3 pixel_corner;
    pixel_corner.setZero();
    viewport()->GetPixelCorner(row, col, &pixel_corner);

    pts.row(0) = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, 1>>(rook_x_.data(), num_samples_per_pixel_);
    pts.row(1) = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, 1>>(rook_y_.data(), num_samples_per_pixel_);
    pts.row(2).setZero();

    double scale = viewport()->s() / num_samples_per_pixel_;

    Vec2 rand(2, pts.cols());
    rand.setRandom();
    rand *= 0.5;
    rand.array() += 0.5;

    pts.topRows(2).colwise() += rand;
    pts.topRows(2) *= scale;
    pts.colwise() += pixel_corner;

    *points = pts;
  }
}

// Maintains the N-rook arrangement after shuffling x coordinates.
void MultiJittered::SetRookY() {
  int k = 0;
  rook_y_ = rook_x_;
  for (int i = 0; i < grid_size_; ++i) {
    for (int j = 0; j < grid_size_; ++j) {
      rook_y_[i + grid_size_ * j] = rook_x_[k];
      ++k;
    }
  }
}

void MultiJittered::GenerateSamplePool(int pool_size) {
  double scale = viewport()->s() / num_samples_per_pixel_;

  for (int i = 0; i < pool_size; ++i) {
    ShuffleRookCoords();

    Points3d pts(3, num_samples_per_pixel_);

    pts.row(0) = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, 1>>(rook_x_.data(), num_samples_per_pixel_);
    pts.row(1) = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, 1>>(rook_y_.data(), num_samples_per_pixel_);
    pts.row(2).setZero();

    Vec2 rand(2, pts.cols());
    rand.setRandom();
    rand *= 0.5;
    rand.array() += 0.5;

    pts.topRows(2).colwise() += rand;
    pts.topRows(2) *= scale;

    pool_.push_back(pts);
  }
}
}
