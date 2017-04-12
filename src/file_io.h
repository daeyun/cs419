//
// Created by daeyun on 4/11/17.
//

#ifndef CS419_CPP_FILE_IO_H
#define CS419_CPP_FILE_IO_H

#include "common.h"

namespace render {

/**
 * Saves an OpenCV image as PNG file.
 *
 * @param filename
 * @param image RGBA image array of pixel values from 0 to 1.
 */
void SavePng(const std::string &filename, const cv::Mat &image);
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
void SerializeRgba(const std::string &filename, const cv::Mat &image);
void SerializeMatrix(const std::string &filename, const Eigen::MatrixXd &mat);
}

#endif //CS419_CPP_FILE_IO_H
