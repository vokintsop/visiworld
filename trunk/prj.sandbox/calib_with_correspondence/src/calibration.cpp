#include "calibration.h"
#include "init.h"
#include <opencv2/imgproc/imgproc.hpp>

#include <ocvutils/hcoords.h> 
#include <cover_net/cover_net.h>


using namespace std;
using namespace cv;

bool OpenCaptures(int lIndex, int rIndex, VideoCapture &capL, VideoCapture &capR)
{
  int forceWidth = 1280, forceHeight = 720;
  capL.open(lIndex);
  capR.open(rIndex);
  capL.set(CV_CAP_PROP_FRAME_WIDTH, forceWidth);
  capL.set(CV_CAP_PROP_FRAME_HEIGHT, forceHeight);
  capR.set(CV_CAP_PROP_FRAME_WIDTH, forceWidth);
  capR.set(CV_CAP_PROP_FRAME_HEIGHT, forceHeight);
  return capL.isOpened() && capR.isOpened();
}

bool OpenCaptures(const string &fname_pattern, VideoCapture &capL, VideoCapture &capR)
{
  string lFname = format(fname_pattern.c_str(), "left");
  string rFname = format(fname_pattern.c_str(), "right");
  capL.open(lFname);
  capR.open(rFname);
  return capL.isOpened() && capR.isOpened();
}

bool ReadLeftAndRightFrames(VideoCapture &capL, VideoCapture &capR, Mat &left, Mat &right)
{
  if (!capL.isOpened() || !capR.isOpened())
    return false;

  capL >> left; flip(left, left, -1);
  capR >> right; flip(right, right, -1);
  return !left.empty() && !right.empty();
}

bool FindCorrespondingPoints(Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame, PointCorType &pointsCorrespondence)
{
  KeyPointDescriptorRuler ruler;
  double rootRadius = 5.0;
  double minRadius = 0.05;
  Ptr<CNType> coverNet = new CNType(&ruler, rootRadius, minRadius);
  InitCoverNet(coverNet, lFrame);
  double dist = 0.1;
  for (unsigned int i = 0; i < rFrame->kps.size(); ++i)
  {
    int iSph = coverNet->findNearestSphere(make_pair(rFrame.obj, i), dist);
    if (iSph != -1)
    {
      int ikp = coverNet->getSphere(iSph).center.second;
      pointsCorrespondence.push_back(make_pair(ikp, i));
    }
  }
  return true;
}


bool DrawCorrPoints(cv::Ptr<SimpleFrame> &lFrame, cv::Ptr<SimpleFrame> &rFrame, PointCorType &pointsCorrespondence)
{
  Mat preres;
  //cout << lFrame->src.size() << "      " << rFrame->src.size() << endl;
  hconcat(lFrame->src, rFrame->src, preres);
  //cout << pointsCorrespondence.size() << endl;
  for (unsigned int i = 0; i < pointsCorrespondence.size(); ++i)
  {
    Point lPoint = lFrame->kps[pointsCorrespondence[i].first].pt;
    Point rPoint = rFrame->kps[pointsCorrespondence[i].second].pt;
    
    cv::circle(preres, lPoint, 1, cv::Scalar(255,0,0), 2);
    cv::circle(preres, Point(rPoint.x + lFrame->src.cols, rPoint.y), 1, cv::Scalar(255,0,0), 2);
    
    cv::line(preres, lPoint, Point(rPoint.x + lFrame->src.cols, rPoint.y), cv::Scalar(0,0,255), 1);
  }

  Mat todraw;
  resize(preres, todraw, Size(), 1400.0 / preres.cols, 1400.0 / preres.cols);
  imshow("window", todraw);
  return true;
}