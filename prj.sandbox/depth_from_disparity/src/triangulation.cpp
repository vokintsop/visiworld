#include "triangulation.h"

#include <iostream>

int TriangulatePoint(
  cv::Mat &res,
  cv::Point2f pixel_1,
  cv::Point2f pixel_2,
  const cv::Mat &camera_matrix_1,
  const cv::Mat &camera_matrix_2)
{
  cv::Mat line1, line2;
  GetProjectionLine(line1, pixel_1, camera_matrix_1);
  GetProjectionLine(line2, pixel_2, camera_matrix_2);

  //std::cout << line1 << std::endl << line2 << std::endl;
  cv::Mat d = line1.col(1) - line2.col(1);
  //dot products:
  double da1 = line1.col(0).dot(d);
  double da2 = line2.col(0).dot(d);
  double a1a2 = line2.col(0).dot(line1.col(0));
  //line dots:
  double s1 = 
    - (da1 + da2 * a1a2 - 2 * da2 * a1a2) / (1 - a1a2 * a1a2);
  cv::Mat y1 = line1.col(1) + s1 * line1.col(0);
  cv::Mat y2 = line2.col(1) + line2.col(0) * line2.col(0).dot(y1 - line2.col(1));
  //std::cout << y1 << std::endl << y2 << std::endl;
  //triangulated point is mean between two closest line points
  res = 0.5 * (y1 + y2);
  return 0;
}

int GetProjectionLine(
  cv::Mat &line,
  cv::Point2f pixel, 
  const cv::Mat &camera_matrix)
{
  cv::Point2f center_pixel;
  float f = camera_matrix.at<float>(0, 0);
  if (f <= 0)
    return -1;
 // std::cout << camera_matrix << std::endl;
  center_pixel.x = camera_matrix.at<float>(0, 2);
  center_pixel.y = camera_matrix.at<float>(1, 2);
  cv::Mat rot_submatrix = camera_matrix.colRange(0, 3);
  cv::Mat trans_part = camera_matrix.col(3);
  cv::Mat point_cartesian(cv::Point3f(pixel.x, pixel.y, 1.0));
  cv::Mat dirvect = (rot_submatrix.inv() * point_cartesian);
  dirvect /= norm(dirvect);
  cv::Mat new_tr_part = -rot_submatrix.inv() * trans_part;
  line = cv::Mat(3, 2, CV_32F);
  dirvect.copyTo(line(cv::Range::all(), cv::Range(0,1)));
  new_tr_part.copyTo(line(cv::Range::all(), cv::Range(1,2)));
  //std::cout << line << std::endl;
  return 0;
}