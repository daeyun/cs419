//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_SAMPLING_H
#define CS419_CPP_SAMPLING_H

#include <memory>

#include "common.h"
#include "viewing.h"

namespace render {

class Sampler {
 public:
  Sampler(std::shared_ptr<Viewport> viewport) : viewport_(viewport) {}

  virtual void Sample(int row, int col, Points3d *points) = 0;

  std::shared_ptr<Viewport> viewport() { return viewport_; };

 protected:
  std::shared_ptr<Viewport> viewport_;

};

class MultiJittered : public Sampler {
 public:
  MultiJittered(const std::shared_ptr<Viewport> &viewport, int grid_size);

  // Shuffles the x, y indices of a grid in a way that satisfies the rook condition.
  void ShuffleRookCoords();

  // Generates 3D points that correspond to a pixel at (y, x).
  void Sample(int row, int col, Points3d *points);

  const std::shared_ptr<Viewport> &viewport() const {
    return viewport_;
  }

  int grid_size() const {
    return grid_size_;
  }

  const std::vector<double> &rook_y() const {
    return rook_y_;
  }

  const std::vector<double> &rook_x() const {
    return rook_x_;
  }

  int num_pixel_points() const {
    return num_samples_per_pixel_;
  }

 private:
  // Maintains the N-rook arrangement after shuffling x coordinates.
  void SetRookY();

  int grid_size_;
  int num_samples_per_pixel_;
  std::vector<double> rook_y_;
  std::vector<double> rook_x_;

  std::default_random_engine rand_;
  std::uniform_real_distribution<double> distribution_;
};
}

#endif //CS419_CPP_SAMPLING_H
