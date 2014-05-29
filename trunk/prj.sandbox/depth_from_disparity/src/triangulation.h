#ifndef TRIANGULATION_H_INCLUDED
#define TRIANGULATION_H_INCLUDED

#include <opencv2/core/core.hpp>


/**
 * @brief Get 3D point by triangulation of 2 camera points
 * @param [out] res - 3d point in CAMERA coordinates.
 * @param [in] pixel_1 - pixel on 1st camera
 * @param [in] pixel_2 - pixel on 2nd camera
 * @param [in] camera_matrix_1 - 3x4 camera 1 matrix
 * @param [in] camera_matrix_2 - 3x4 camera 2 matrix
 * @returns @c #MinErr
*/
int TriangulatePoint(
  cv::Mat &res,
  cv::Point2d pixel_1,
  cv::Point2d pixel_2,
  const cv::Mat &camera_matrix_1,
  const cv::Mat &camera_matrix_2);

/**
 * @brief Line that projects into given pixel
 * @param [out] line - 3x2 matrix; 1st column - line direction vector, 
    2nd column - some point on the line. EVERYTHING IN CAMERA COORDINATES
 * @param [in] pixel - projection pixel
 * @param [in] camera_matrix - 3x4 camera matrix
 * @returns @c #MinErr
*/
int GetProjectionLine(
  cv::Mat &line,
  cv::Point2d pixel, 
  const cv::Mat &camera_matrix);

#endif //TRIANGULATION_H_INCLUDED