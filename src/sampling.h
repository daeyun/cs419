//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_SAMPLING_H
#define CS419_CPP_SAMPLING_H

#include <memory>

#include "common.h"
#include "viewing.h"

namespace render {

class Viewport;

static void RotationAlignZ(const Vec3 &z_axis, Vec3 *x, Vec3 *y) {
  if (std::abs(z_axis[0]) > std::abs(z_axis[1])) {
    *x = Vec3(z_axis[2], 0, -z_axis[0])
        / sqrt(z_axis[0] * z_axis[0] + z_axis[2] * z_axis[2]);
  } else {
    *x = Vec3(0, -z_axis[2], z_axis[1])
        / sqrt(z_axis[1] * z_axis[1] + z_axis[2] * z_axis[2]);
  }
  *y = z_axis.cross(*x);
}

class PixelSampler {
 public:
  PixelSampler(const Viewport &viewport) : viewport_(viewport) {}

  virtual void Sample(int row, int col, Points3d *points) = 0;

  const Viewport &viewport() { return viewport_; };

 protected:
  Viewport viewport_;

};

class MultiJittered : public PixelSampler {
 public:
  MultiJittered(const Viewport &viewport, int grid_size);

  // Shuffles the x, y indices of a grid while satisfying the rook condition.
  void ShuffleRookCoords();

  // Generates 3D points that correspond to a pixel at (y, x).
  void Sample(int row, int col, Points3d *points);

  const Viewport &viewport() const {
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

  void GenerateSamplePool(int pool_size);

  int grid_size_;
  int num_samples_per_pixel_;
  std::vector<double> rook_y_;
  std::vector<double> rook_x_;

  const int default_pool_size_ = 5000;
  std::vector<Points3d> pool_;

  std::default_random_engine rand_;
  std::uniform_int_distribution<> distribution_;
};

class HemisphereSampler {
 public:
  HemisphereSampler(double exp, int pool_size = 20000)
      : exp_(exp),
        pool_size_(pool_size),
        hemisphere_sample_pool_(3, pool_size) {
    exp_1_ = exp_ + 1.0;
    inv_exp_1_ = 1.0 / exp_1_;

    std::uniform_real_distribution<double> distribution(0, 1);

    for (int i = 0; i < pool_size; ++i) {
      double x = distribution(rand_);
      double y = distribution(rand_);

      double phi = 2.0 * M_PI * x;
      double cos_phi = std::cos(phi);
      double sin_phi = std::sin(phi);
      double cos_theta = std::pow(y, inv_exp_1_);
      double sin_theta = std::sqrt(1 - cos_theta * cos_theta);

      double u = sin_theta * cos_phi;
      double v = sin_theta * sin_phi;
      double w = cos_theta;

      hemisphere_sample_pool_(0, i) = u;
      hemisphere_sample_pool_(1, i) = v;
      hemisphere_sample_pool_(2, i) = w;

      double pdf = (exp_1_) * pow(cos_theta, exp) * kOneOverTwoPi;
      CHECK(pdf > 0);
      pdf_pool_.push_back(pdf);
    }
  }

  void Sample(Vec3 *sampled, double *pdf) const {
    int ind = std::uniform_int_distribution<int>{0, pool_size_ - 1}(rand_);

    ChooseSampleFromPool(ind, sampled);
    *pdf = pdf_pool_[ind];
  }

  void Sample(const Vec3 &n, Vec3 *sampled, double *pdf) const {
    Vec3 point;
    Sample(&point, pdf);
    Vec3 x, y;
    RotationAlignZ(n, &x, &y);
    *sampled = x * point(0) + y * point(1) + n * point(2);
  }

  void SampleMany(const Vec3 &n, int num, Points3d *sampled, Vec *pdf) const {
    sampled->resize(3, num);
    pdf->resize(num, 1);
    Points3d points(3, num);

    auto dist = std::uniform_int_distribution<int>{0, pool_size_ - 1};

    // TODO(daeyun): avoid repetition.
    for (int i = 0; i < num; ++i) {
      const int ind = dist(rand_);
      Vec3 p;
      ChooseSampleFromPool(ind, &p);
      points.col(i).array() = p;
      (*pdf)(i) = pdf_pool_[ind];
    }

    Vec3 x, y;
    RotationAlignZ(n, &x, &y);
    *sampled = x * points.row(0) + y * points.row(1) + n * points.row(2);
  }

  void ChooseSampleFromPool(int i, Vec3 *sample) const {
    *sample = const_cast<Points3d *>(&hemisphere_sample_pool_)->col(i).eval();
  }

  void set_hemisphere_sample_pool(const Points3d &hemisphere_sample_pool) {
    hemisphere_sample_pool_ = hemisphere_sample_pool;
  }

 private:
  double exp_;
  double exp_1_;
  double inv_exp_1_;
  const int pool_size_;
  Points3d hemisphere_sample_pool_;
  std::vector<double> pdf_pool_;
  mutable std::default_random_engine rand_;
};

}

#endif //CS419_CPP_SAMPLING_H
