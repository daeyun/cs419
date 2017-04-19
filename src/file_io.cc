//
// Created by daeyun on 4/11/17.
//

#include <chrono>
#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "file_io.h"

namespace render {

/**
 * Saves an OpenCV image as PNG file.
 *
 * @param filename
 * @param image RGBA image array of pixel values from 0 to 1.
 */
void SavePng(const std::string &filename, const cv::Mat &image) {
  cv::Mat image1;
  image.convertTo(image1, CV_8UC4, 255.0);
  cv::Mat image2;
  cvtColor(image1, image2, CV_RGBA2BGRA);
  imwrite(filename, image2);
}

/**
 * Saves an OpenCV array as a raw binary file.
 *
 * Format:
 *  int32 array shape values [n, dim_1, dim_2, ..., dim_n]
 *  followed by [data]
 *
 * @param filename
 * @param image RGBA image array.
 */
void SerializeRgba(const std::string &filename, const cv::Mat &image) {
  std::ofstream file;
  file.open(filename, std::ios_base::out | std::ios_base::binary);

  cv::Mat image_float;
  image.convertTo(image_float, CV_32FC4);

  int32_t num;
  num = static_cast<int32_t>(image_float.dims) + 1;
  file.write(reinterpret_cast<const char *>(&num), sizeof(num));

  for (int i = 0; i < image_float.dims; ++i) {
    num = static_cast<int32_t>(image_float.size[i]);
    file.write(reinterpret_cast<const char *>(&num), sizeof(num));
  }

  num = static_cast<int32_t>(4);
  file.write(reinterpret_cast<const char *>(&num), sizeof(num));

  for (size_t i = 0; i < image_float.total(); ++i) {
    auto &pixel = image_float.at<cv::Vec4f>(i);
    file.write(reinterpret_cast<const char *>(pixel.val), sizeof(pixel.val));
  }
}

void SerializeMatrix(const std::string &filename, const Eigen::MatrixXd &mat) {
  std::ofstream file;
  file.open(filename, std::ios_base::out | std::ios_base::binary);

  auto floats = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>(mat.cast<float>());

  int32_t num = 2;
  file.write(reinterpret_cast<const char *>(&num), sizeof(num));

  num = static_cast<int32_t>(floats.rows());
  file.write(reinterpret_cast<const char *>(&num), sizeof(num));

  num = static_cast<int32_t>(floats.cols());
  file.write(reinterpret_cast<const char *>(&num), sizeof(num));

  file.write(reinterpret_cast<const char *>(floats.data()), floats.size() * sizeof(float));
}
}