#ifndef MAPTOREALWORLD_H__
#define MAPTOREALWORLD_H__
#pragma once

#include <opencv2/core/core.hpp>
#include <gnss/gnss.h>

cv::Point2d Mercator(cv::Point2d nePt);
cv::Point2d Mercator(const GNSSRecord &nePt);


//
struct CameraOnMap
{
  cv::Point2d origin;   //meteres
  cv::Vec2d direction;  //meteres
  cv::Mat intrinsics;
};

/*
  Matrix of affine transform to transform global map points to map points in camera coordinate
  system
*/
cv::Mat CalcAffineTransform(cv::Point2d origin, cv::Point2d pt, cv::Vec2d dir);

/*
  Transform map points coordinates to camera-centered and directed coordinate system
*/
void TransformMapPointCoordinates(const cv::Mat &affineMat,
  const std::vector<cv::Point2d> &mapPoints, std::vector<cv::Point2d> &resMapPoints);

void GetImagePoints(const std::vector<cv::Point2d> &mapPoints, const cv::Mat &intrinsics,
  std::vector<cv::Point> &imgPts);

void drawMapPointsOnImage(const std::vector<cv::Point2d> enuMapPoints, const CameraOnMap& cam, 
  cv::Mat &todraw);

class Camera2DPoseEstimator
{
public:
  Camera2DPoseEstimator(const NMEA &nmea_, const cv::Mat &intrinsics_)
    : nmea(nmea_), intrinsics(intrinsics_) 
  {
    //LinearEstimatePose();
    EstimatePoseWithOxtsYaw();
  }

  CameraOnMap GetPoseAtTime(double time);


private:
  const NMEA &nmea;
  cv::Mat intrinsics;
  std::vector<cv::Vec2d> directions;

  void LinearEstimatePose();
  void EstimatePoseWithOxtsYaw();
};
#endif //MAPTOREALWORLD_H__