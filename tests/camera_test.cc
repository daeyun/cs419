//
// Created by daeyun on 4/11/17.
//

#include "gtest/gtest.h"
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include "src/matrix_utils.h"
#include "src/common.h"
#include "src/viewing.h"

using namespace render;

TEST(SimpleTest, SimpleTest) {
  Viewport viewport(2, 2);
  Points2d points;
  viewport.GetPixelCenters(&points);

  EXPECT_EQ(4, points.cols());
  EXPECT_DOUBLE_EQ(-0.5, points.minCoeff());
  EXPECT_DOUBLE_EQ(0.5, points.maxCoeff());
  EXPECT_DOUBLE_EQ(0.5, points.cwiseAbs().minCoeff());
}

