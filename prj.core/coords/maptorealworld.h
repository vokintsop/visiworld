#ifndef MAPTOREALWORLD_H__
#define MAPTOREALWORLD_H__
#pragma once

#include <opencv2/core/core.hpp>
#include <gnss/gnss.h>

#include <prj.sandbox/markup/src/pose.h>

cv::Point2d Mercator(cv::Point2d nePt);
cv::Point2d Mercator(const GNSSRecord &nePt);



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
  cv::Vec3d t, std::vector<cv::Point> &imgPts);

void drawMapPointsOnImage(const std::vector<cv::Point2d> &enuMapPoints, const CameraPose& cam, 
  cv::Mat &todraw);




class Camera2DPoseEstimator
{
public:
  Camera2DPoseEstimator(const NMEA &nmea_, const cv::Mat &intrinsics_, const cv::Mat &t_, bool iskitti = true)
    : nmea(nmea_), intrinsics(intrinsics_), t(t_)
  {
   // if (iskitti)
      EstimatePoseWithOxtsYaw();
    //else
     // LinearEstimatePose();
  }

  CameraPose GetPoseAtTime(double time);


private:
  const NMEA &nmea;
  cv::Mat intrinsics;
  cv::Mat t;
  std::vector<cv::Vec2d> directions;

  void LinearEstimatePose();
  void EstimatePoseWithOxtsYaw();
};

class CoordinateTransformer
{
public:

  CoordinateTransformer(const CameraPose &pose) :camPose(pose) {}     


  /*
    [in] mapPoints - набор EN-точек
    [out] imgPts - набор координат точек в плоскости изображения
  */
  bool GetImagePointsFromMapPoints(const std::vector<cv::Point2d> &mapPoints,
    std::vector<cv::Point2d> &imgPts);

  bool GetMapPointsFromImagePointsOnGround(const std::vector<cv::Point2d> &groundImgPoints,
    std::vector<cv::Point2d> &mapPoints);






private:
  CameraPose camPose;

  bool MapPointsToCameraCoordinates(const std::vector<cv::Point2d> &mapPoints,
    std::vector<cv::Point3d> &points3D);
  bool Project3DPointsToImage(const std::vector<cv::Point3d> &points3D,
    std::vector<cv::Point2d> &imgPts);
};

void FillPoseMats(const cv::Mat &base_R, const cv::Mat &base_t, CameraPose &cam);
//{
  //cam.


//}


void DrawMapObjectsOnFrame(cv::Mat &img, 
  const std::vector<cv::Point2d> &enPts, 
  const std::vector<std::pair<cv::Point2d, cv::Point2d> > &enSegms,
  const CameraPose &cam);
#endif //MAPTOREALWORLD_H__