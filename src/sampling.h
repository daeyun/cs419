//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_SAMPLING_H
#define CS419_CPP_SAMPLING_H

#include <memory>

#include "common.h"
#include "viewing.h"

namespace render {

class PixelSampler {
 public:
  PixelSampler(std::shared_ptr<Viewport> viewport) : viewport_(viewport) {}

  virtual void Sample(int row, int col, Points3d *points) = 0;

  std::shared_ptr<Viewport> viewport() { return viewport_; };

 protected:
  std::shared_ptr<Viewport> viewport_;

};

class MultiJittered : public PixelSampler {
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
      : exp_(exp), pool_size_(pool_size), hemisphere_sample_pool_(3, pool_size), int_distribution_(0, pool_size - 1) {
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
      pdf_pool_.push_back(pdf);
    }
  }

  void Sample(Vec3 *sampled, double *pdf) {
    int ind = int_distribution_(rand_);
    *sampled = hemisphere_sample_pool_.col(ind);
    *pdf = pdf_pool_[ind];
  }

  void Sample(const Vec3 &n, Vec3 *sampled, double *pdf) {
    Sample(sampled, pdf);
    Mat33 rot;
    RotationAlignZ(n, &rot);
    *sampled = rot * (*sampled);
  }

  void SampleMany(const Vec3 &n, int num, Points3d *sampled, Vec *pdf) {
    sampled->resize(3, num);
    pdf->resize(1, num);
    // TODO(daeyun): avoid repetition.
    for (int i = 0; i < num; ++i) {
      Vec3 v;
      double pdf_i;
      Sample(&v, &pdf_i);
      sampled->col(i).array() = v;
      (*pdf)(i) = pdf_i;
    }
    Mat33 rot;
    RotationAlignZ(n, &rot);
    *sampled = rot * (*sampled);
  }

 private:
  double exp_;
  double exp_1_;
  double inv_exp_1_;
  const int pool_size_;
  Points3d hemisphere_sample_pool_;
  std::vector<double> pdf_pool_;
  std::default_random_engine rand_;
  std::uniform_int_distribution<> int_distribution_;
};

}

#endif //CS419_CPP_SAMPLING_H
