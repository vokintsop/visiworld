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
  cv::Vec2d direction;  //Map image coordinates(x: from left to right, y: from up to down)
  
  //3d
  cv::Matx33d intrinsics;
  cv::Matx33d R;
  cv::Vec3d t;

  cv::Matx34d GetProjectionMatrix();

  bool FillExtrinsics();
};




bool UpdateCameraPose(const CameraPose &initialPose,
  const std::vector<cv::Point> &detectedObjects,      //pixels
  const std::vector<cv::Point2d> mapPoints);          //meters

#endif //__POSE_H