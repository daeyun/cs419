//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_COMMON_H
#define CS419_CPP_COMMON_H

#include <iostream>
#include <limits>
#include <cstdlib>

#include <opencv2/core/core.hpp>
#include <Eigen/Dense>

#define CHECK(condition) if(__builtin_expect(!(condition), 0)) {std::cerr \
<< std::endl << __FILE__ << ":" <<  __LINE__ << (":: Condition " #condition " failed. "); std::abort();}

using Vec = Eigen::VectorXd;
using Vec3 = Eigen::Vector3d;
using Vec2 = Eigen::Vector2d;
using Mat44 = cv::Matx44d;
using Mat34 = cv::Matx34d;
using Mat33 = cv::Matx33d;

using Points3d = Eigen::Matrix<double, 3, Eigen::Dynamic>;
using Points2d = Eigen::Matrix<double, 2, Eigen::Dynamic>;
using Points2i = Eigen::Matrix<int, 2, Eigen::Dynamic>;

extern double infinity;

#endif //CS419_CPP_COMMON_H
