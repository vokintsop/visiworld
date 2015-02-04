#ifndef __POSE_H
#define __POSE_H

#pragma once

#include <vector>
#include <opencv2/core/core.hpp>

//
struct CameraPose
{
  //on map
  cv::Point2d origin;   //meteres
  cv::Vec2d direction;  //meteres
  
  //3d
  cv::Mat intrinsics;
  cv::Mat R;
  cv::Mat t;

  inline cv::Mat GetProjectionMatrix()
  {
    //return (I|0) * (R|t)
    cv::Mat initMat;
    cv::hconcat(intrinsics, cv::Mat::zeros(3, 1, intrinsics.type()), initMat);
    cv::Mat tmp, transMat;
    cv::hconcat(R, t, tmp);
    cv::Mat botLine = (cv::Mat_<double>(1, 4) << 0, 0, 0, 1);
    cv::vconcat(tmp, botLine, transMat);

    return initMat * transMat;
  }
};




bool UpdateCameraPose(const CameraPose &initialPose,
  const std::vector<cv::Point> &detectedObjects,      //pixels
  const std::vector<cv::Point2d> mapPoints);          //meters

#endif //__POSE_H