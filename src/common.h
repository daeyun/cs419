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

using Eigen::Dynamic;
using Vec = Eigen::VectorXd;
using Vec2 = Eigen::Vector2d;
using Vec3 = Eigen::Vector3d;
using Vec4 = Eigen::Vector4d;
using Mat44 = Eigen::Matrix<double, 4, 4>;
using Mat34 = Eigen::Matrix<double, 3, 4>;
using Mat33 = Eigen::Matrix<double, 3, 3>;

using Points3d = Eigen::Matrix<double, 3, Eigen::Dynamic>;
using Points2d = Eigen::Matrix<double, 2, Eigen::Dynamic>;
using Points2i = Eigen::Matrix<int, 2, Eigen::Dynamic>;

constexpr double kInfinity = std::numeric_limits<double>::infinity();
constexpr double kOneOverTwoPi = M_1_PI * 0.5;

#endif //CS419_CPP_COMMON_H
