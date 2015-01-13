#include "correspond.h"
#include "init.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/flann/flann.hpp>

#include <iostream>
#include <vector>


using namespace std;
using namespace cv;


void DrawKeyPoints(Mat &todraw, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame)
{
  for (unsigned int i = 0; i < lFrame->kps.size(); ++i)
    circle(todraw, lFrame->kps[i].pt, 1, Scalar(0,0,255)); 

  for (unsigned int i = 0; i < rFrame->kps.size(); ++i)
    circle(todraw, Point(rFrame->kps[i].pt.x + lFrame->src.cols, rFrame->kps[i].pt.y), 1, Scalar(0,0,255)); 
}

void CorrespondStereo(KeyPointDescriptorRuler *ruler, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame, PointMatches &pm)
{
  vector<pair<Point, Point> > pointMatch;
  Size sz = lFrame->src.size();
  cv::Mat todraw = Mat::zeros(Size(sz.width * 2, sz.height), CV_8UC3);
  lFrame->src.copyTo(todraw(Rect(Point(0,0), sz)));
  rFrame->src.copyTo(todraw(Rect(Point(sz.width,0), sz)));
  DrawKeyPoints(todraw, lFrame, rFrame);

  //CorrespondStereoWithCNet(ruler, lFrame, rFrame, pm, todraw);
  CorrespondStereoWithFlann(ruler, lFrame, rFrame, pm, todraw);
  //CorrespondStereoWithMinimalDist(ruler, lFrame, rFrame, pm, todraw);

  cv::Mat rsz;
  line(todraw, Point(0,200), Point(todraw.cols - 1, 200), Scalar(0,255,0));
  resize(todraw, rsz, Size(), 1600.0 / todraw.cols, 1600.0 / todraw.cols);
  imshow("cvnet", rsz);
}

void CorrespondStereoWithCNet(
  KeyPointDescriptorRuler *ruler,
  Ptr<SimpleFrame> &lFrame, 
  Ptr<SimpleFrame> &rFrame,
  PointMatches &pm, 
  Mat &todraw)
{
  Ptr<CNType> coverNet(new CNType(ruler, 3.0 * lFrame->sigmaDescr, 0.05 * lFrame->sigmaDescr));
  InitCoverNet(coverNet, lFrame);

  double dist = 1.025 * lFrame->sigmaDescr;
  cout << "dist for nearestSearch: " << dist << endl;
  int matchNumber = 0;
  for (unsigned int i = 0; i < rFrame->kps.size(); ++i)
  {
    Point pt = rFrame->kps[i].pt;    
    int iSph = coverNet->findNearestSphere(make_pair(rFrame.obj, i), dist);
    if (iSph != -1)
    {
      ++matchNumber;
      int ikp = coverNet->getSphere(iSph).center.second;
      Point right = pt;
      Point left = lFrame->kps[ikp].pt;
      pm.push_back(make_pair(left, right));
      pt = Point(pt.x + lFrame->src.cols, pt.y);
      if (!todraw.empty())
      {
        circle(todraw, pt, 1, cv::Scalar(255,0,0), 2);
        circle(todraw, left, 1, cv::Scalar(255,0,0), 2);
        line(todraw, left, pt, cv::Scalar(255,0,0), 2);
      }
    }
  }
  cout << "matches with coverTree:\t" << matchNumber << endl;
  coverNet->reportStatistics();
  cout << endl << endl;
  return;
}

void CorrespondStereoWithMinimalDist(
  KeyPointDescriptorRuler *ruler,
  Ptr<SimpleFrame> &lFrame, 
  Ptr<SimpleFrame> &rFrame, 
  PointMatches &pm,
  Mat &todraw)
{
  double distthresh = 0.1 * lFrame->sigmaDescr;
  int matchNumber = 0;
  if (rFrame->kps.empty() || lFrame->kps.empty())
    return;

  for (int i = 0; i < rFrame->kps.size(); ++i)
  {
    int jmin = 0;
    double mindist = ruler->computeDistance(make_pair(lFrame, 0), make_pair(rFrame, i));
    for (int j = 1; j < lFrame->kps.size(); ++j)
    {
      double dist = ruler->computeDistance(make_pair(lFrame, j), make_pair(rFrame, i));
      if (dist < mindist)
      {
        mindist = dist;
        jmin = j;
      }
    }
    if (mindist < distthresh)
    {
      ++matchNumber;
      Point right = rFrame->kps[i].pt;
      Point left = lFrame->kps[jmin].pt;
      pm.push_back(make_pair(left, right));
      right.x += lFrame->src.cols;
      if (!todraw.empty())
      {
        circle(todraw, right, 1, cv::Scalar(255,0,0), 2);
        circle(todraw, left, 1, cv::Scalar(255,0,0), 2);
        line(todraw, left, right, cv::Scalar(255,0,0), 2);
      }
    }
  }
  cout << "matches with minimalDist:\t" << matchNumber << endl;
  cout << endl << endl;
  return;
}

void CorrespondStereoWithFlann(
  KeyPointDescriptorRuler *ruler,
  Ptr<SimpleFrame> &lFrame,
  Ptr<SimpleFrame> &rFrame, 
  PointMatches &pm, 
  Mat &todraw)
{
  double distthresh = 0.1 * lFrame->sigmaDescr;
  int matchNumber = 0;
  flann::Index flannIndex(lFrame->descriptors, flann::LinearIndexParams());
  Mat indices, dists;

  flannIndex.knnSearch(rFrame->descriptors, indices, dists, 1);
  //cout << indices << endl;// << dists << endl;
  for (int i = 0; i < rFrame->kps.size(); ++i)
  {
    if (dists.at<float>(i,0) < distthresh)
    {
      ++matchNumber;
      Point left = lFrame->kps[indices.at<int>(i,0)].pt;
      Point right = rFrame->kps[i].pt;
      pm.push_back(make_pair(left, right));
      right.x += lFrame->src.cols;
      if (!todraw.empty())
      {
        circle(todraw, right, 1, cv::Scalar(255,0,0), 2);
        circle(todraw, left, 1, cv::Scalar(255,0,0), 2);
        line(todraw, left, right, cv::Scalar(255,0,0), 2);
      }
    }
  }
  cout << "matches with FlannLinearRadiusSearch:\t" << matchNumber << endl;
  cout << endl << endl;
}