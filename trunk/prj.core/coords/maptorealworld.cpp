#include "maptorealworld.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;

static const double R = 6378137.0; //meters
static const double iR = 1 / R;//6378137.0; //meters

inline Point2d Mercator(Point2d nePt)
{
  Point2d result;
  result.x = R * M_PI * nePt.x / 180;
  result.y = R * log(tan(M_PI * (90 + nePt.y) / 360));
  return result;
}

inline Point2d Mercator(const GNSSRecord &nePt)
{
  return Mercator(Point2d(nePt.east, nePt.nord));
}

static inline Point2d iMercator(Point2d meterPoint, Point2d origin = Point2d())
{
  Point2d nePt;
  meterPoint.x -= origin.x;
  meterPoint.y -= origin.y;
  nePt.x = iR * meterPoint.x * 180 / M_PI;
  nePt.y = atan(exp(iR * meterPoint.y)) * 360 / M_PI - 90;
  return nePt;
}

Mat CalcAffineTransform(Point2d origin, Point2d pt, Vec2d dir)
{
  dir = dir / norm(dir);
  Mat t = (Mat_<double>(2, 1) << pt.x - origin.x, pt.y - origin.y);
  Mat R = (Mat_<double>(2, 2) << dir[1], -dir[0], dir[0], dir[1]);
  Mat A = Mat::zeros(2, 3, R.type());

  R.copyTo(A(Range(0, 2), Range(0, 2)));
  Mat(R * t).copyTo(A(Range(0, 2), Range(2, 3)));

  return A;
}

void TransformMapPointCoordinates(const Mat &affineMat,
  const vector<Point2d> &mapPoints, vector<Point2d> &resMapPoints)
{
  if (affineMat.size() != Size(3, 2))
    return;

  Mat pointMat(3, mapPoints.size(), CV_64F);
  for (unsigned int i = 0; i < mapPoints.size(); ++i)
    Mat(Vec3d(mapPoints[i].x, mapPoints[i].y, 1), false).copyTo(pointMat.col(i));
  
  Mat resmat = affineMat * pointMat;
  for (int i = 0; i < resmat.cols; ++i)
    resMapPoints.push_back(Point2d(resmat.at<double>(0, i), resmat.at<double>(1, i)));
}

void GetImagePoints(const vector<Point2d> &mapPoints, const Mat &intrinsics,
  vector<Point> &imgPts)
{
  if (mapPoints.empty())
  {
    imgPts.clear();
    return;
  }
  Vec3d t(-0.32, 0, -1.08); //Kitti-specific
  Mat pointMat(3, 2 * mapPoints.size(), CV_64F);
  for (unsigned int i = 0; i < mapPoints.size(); ++i)
  {
    //y point coodrinates are Kitti-specific
    Mat(Vec3d(mapPoints[i].x, 2.5, mapPoints[i].y) + t).copyTo(pointMat.col(2 * i));
    Mat(Vec3d(mapPoints[i].x, -1.5, mapPoints[i].y) + t).copyTo(pointMat.col(2 * i + 1));
  }

  Mat projectedPoints = intrinsics * pointMat;
  Mat t0to2 = (Mat_<double>(3,1) << 44.8573, 0.2164, 0.0027);
  Mat toextract;
  repeat(t0to2, 1, pointMat.cols, toextract);
  projectedPoints += toextract;
  for (unsigned int i = 0; i < mapPoints.size(); ++i)
  {
    double z_1 = projectedPoints.at<double>(2, 2 * i);
    double z_2 = projectedPoints.at<double>(2, 2 * i + 1);
    if (z_1 > 0 && z_2 > 0 && z_1 <= 200.)
    {
      imgPts.push_back(Point(cvRound(
        projectedPoints.at<double>(0, 2 * i) / z_1), 
        cvRound(projectedPoints.at<double>(1, 2 * i) / z_1)));
      imgPts.push_back(Point(cvRound(
        projectedPoints.at<double>(0, 2 * i + 1) / z_2), 
        cvRound(projectedPoints.at<double>(1, 2 * i + 1) / z_2)));
    }
  }
}

void drawMapPointsOnImage(const vector<Point2d> enuMapPoints, 
  const CameraPose& cam, Mat &todraw)
{
  vector<Point2d> meterMapPoints(enuMapPoints.size(), Point2d());
  for (unsigned int i = 0; i < enuMapPoints.size(); ++i)
    meterMapPoints[i] = Mercator(enuMapPoints[i]);

  Mat A = CalcAffineTransform(cam.origin, Point2d(0,0), cam.direction);
  //cout << A << endl;
  vector<Point2d> camMapPoints;
  //meterMapPoints.push_back(cam.origin);
  camMapPoints.reserve(meterMapPoints.size());
  TransformMapPointCoordinates(A, meterMapPoints, camMapPoints);
 // cout << camMapPoints[0] << endl;
  vector<Point> imgPts;
  imgPts.reserve(2 * camMapPoints.size());
  GetImagePoints(camMapPoints, cam.intrinsics, imgPts);

  for (unsigned int i = 0; 2 * i < imgPts.size(); ++i)
    line(todraw, imgPts[2 * i], imgPts[2 * i + 1], Scalar(255, 0, 0), 5);
}

void Camera2DPoseEstimator::LinearEstimatePose()
{
  vector<Vec2d> meterCameraPoses(nmea.records.size());
  for (unsigned int i = 0; i < nmea.records.size(); ++i)
  {
    meterCameraPoses[i] = Vec2d(Mercator(nmea.records[i]));
  }
  directions.clear();
  directions.resize(nmea.records.size(), Vec2d(0, 1));
  
  Vec2d nonZeroDir;
  bool firstNonZero = true;
  for (unsigned int i = 0; i < nmea.records.size() - 1; ++i)
  {
    Vec2d dir = meterCameraPoses[i + 1] - meterCameraPoses[i];
    if (norm(dir) != 0)
      nonZeroDir = dir / norm(dir);
    if (norm(nonZeroDir) == 0)
      continue;
    directions[i] = nonZeroDir;
    if (firstNonZero)
    {
      for (unsigned int k = 0; k < i; ++k)
        directions[k] = nonZeroDir;
      firstNonZero = false;
    }
  }
  directions[directions.size() - 1] = nonZeroDir;
}

void Camera2DPoseEstimator::EstimatePoseWithOxtsYaw()
{
  directions.clear();
  directions.reserve(nmea.records.size());
  for (unsigned int i = 0; i < nmea.records.size(); ++i)
  {
    directions.push_back(Vec2d(cos(nmea.records[i].yaw), 
      sin(nmea.records[i].yaw)));
  }  
}

CameraPose Camera2DPoseEstimator::GetPoseAtTime(double time)
{
  vector<GNSSRecord>::const_iterator iter = lower_bound(
    nmea.records.begin(), nmea.records.end(), time, compareByTime);
  if (iter == nmea.records.end())
    return CameraPose();
  unsigned int i = distance(nmea.records.begin(), iter);
  Vec2d dir;
  if (i == directions.size() - 1)
    dir = directions[i];
  else
  {
    double prevtime = iter->time;
    double nexttime = (iter + 1)->time;
    dir = (time - prevtime) / (nexttime - prevtime) * directions[i + 1] +
      (nexttime - time) / (nexttime - prevtime) * directions[i];
  }
  
  CameraPose result;
  result.direction = dir;
  result.intrinsics = intrinsics;
  double east = 0, nord = 0;
  nmea.getEastNord(time, east, nord);
  result.origin = Mercator(Point2d(east, nord));
  return result;
}


bool CoordinateTransformer::GetImagePointsFromMapPoints(
  const vector<Point2d> &mapPoints,
  vector<Point2d> &imgPts)
{
  vector<Point3d> points3D;
  if (!MapPointsToCameraCoordinates(mapPoints, points3D))
    return false;
  if (!Project3DPointsToImage(points3D, imgPts))
    return false;
  return true;
}

bool CoordinateTransformer::MapPointsToCameraCoordinates(
  const vector<Point2d> &mapPoints,
  vector<Point3d> &points3D)
{
  points3D.reserve(mapPoints.size());
  for (unsigned int i = 0; i < mapPoints.size(); ++i)
  {
    Point2d pt = mapPoints[i];
    Point2d meterPt = Mercator(pt);

    meterPt.x -= camPose.origin.x;
    meterPt.y -= camPose.origin.y;

    points3D.push_back(Point3d(meterPt.y, 0, meterPt.x));
  }
  return true;
}

bool CoordinateTransformer::Project3DPointsToImage(
  const vector<Point3d> &points3D,
  vector<Point2d> &imgPts)
{
  //Mat MatOf3DPoints = ; 
  imgPts.reserve(points3D.size());
  Mat projectionMat = camPose.GetProjectionMatrix();
  for (unsigned int i = 0; i < points3D.size(); ++i)
  {
    Mat homo4DPt = (Mat_<double>(4, 1) << 
      points3D[i].x, points3D[i].y, points3D[i].z, 1);
    Mat projHomoPt = projectionMat * homo4DPt;
    double w = projHomoPt.at<double>(2, 0);
    if (w == 0)
      return false;
    imgPts.push_back(Point2d(projHomoPt.at<double>(0, 0) / w, 
      projHomoPt.at<double>(1, 0) / w));
  }
  return true;
}

/*
bool CalculateEquationParameters(const Mat &P, Point2d pt, Mat &A, Mat &b)
{
  A.at<double>(0, 0) = P.at<double>(0, 0) - P.at<double>(2, 0) * pt.x;
  A.at<double>(0, 1) = P.at<double>(0, 1) - P.at<double>(2, 1) * pt.x;
  A.at<double>(1, 0) = P.at<double>(1, 0) - P.at<double>(2, 0) * pt.y;
  A.at<double>(1, 1) = P.at<double>(1, 1) - P.at<double>(2, 0) * pt.y;

  b.at<double>(0, 0) = P.at<double>(2, 3) * pt.x - P.at<double>(0, 3);
  b.at<double>(1, 0) = P.at<double>(2, 3) * pt.y - P.at<double>(1, 3);
}*/

bool CalculateEquationParameters(Matx34d P, Vec2d pt, Matx22d &A, Vec2d &b)
{
  A(0, 0) = P(0, 0) - P(2, 0) * pt(0);
  A(0, 1) = P(0, 1) - P(2, 1) * pt(0);
  A(1, 0) = P(1, 0) - P(2, 0) * pt(1);
  A(1, 1) = P(1, 1) - P(2, 0) * pt(1);

  A(0, 0) = P(2, 3) * pt(0) - P(0, 3);
  A(0, 0) = P(2, 3) * pt(1) - P(1, 3);
  
  return true;
}

bool CoordinateTransformer::GetMapPointsFromImagePointsOnGround(
  const vector<Point2d> &groundImgPoints,
  vector<Point2d> &mapPoints)
{
  mapPoints.reserve(groundImgPoints.size());
 // Mat projMat = camPose.GetProjectionMatrix();
  Matx34d projMat = camPose.GetProjectionMatrix();

  for (unsigned int i = 0; i < groundImgPoints.size(); ++i)
  {
    //Mat A(Size(2, 2), CV_64F), b(Size(2, 1), CV_64F);
    Matx22d A;
    Vec2d b;
    if (!CalculateEquationParameters(projMat, groundImgPoints[i], A, b))
      return false;
    Vec2d pt = A.inv() * b;
    mapPoints.push_back(Point2d(pt));
  }

  /*
    now inverse Mercator so points are EN
  */
  for (unsigned int i = 0; i < mapPoints.size(); ++i)
    mapPoints[i] = iMercator(mapPoints[i], camPose.origin);

  return true;
}