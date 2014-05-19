#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "simpleframe.h"
#include "init.h"

#include <ocvutils/hcoords.h> 
#include <cover_net/cover_net.h>
#include "triangulation.h"




using namespace cv;
using namespace std;

string featureType;
map<string, int> featureDetectorInts;
map<string, double> featureDetectorDoubles;
map<string, bool> featureDetectorBools;
cv::Ptr<FeatureDetector> featureDetector;
cv::Ptr<DescriptorExtractor> descriptorExtractor;


void FillStandartFeatureDetectorParameters()
{
  featureType = "SURF";

  featureDetectorInts["extended"] = 0;
  featureDetectorInts["upright"] = 1; //no rotation
  //featureDetectorInts["nOctaves"] = 4; //default 4
  //featureDetectorInts["nOctaveLayers"] = 2; //default 2

  featureDetectorDoubles["hessianThreshold"] = 8000;
}

/*
void FillStandartFeatureDetectorParameters()
{
  featureType = "BRISK";
  featureDetectorInts["thresh"] = 30; //default 30
  featureDetectorInts["octaves"] = 30; //default 30
  featureDetectorBools["nonmaxSuppression"] = true; //default true
}*/

void InitFeatureDetector()
{
  FillStandartFeatureDetectorParameters();
  initModule_nonfree();
  featureDetector = FeatureDetector::create(featureType);
  for (map<string, int>::const_iterator iter = featureDetectorInts.begin();
    iter != featureDetectorInts.end();
    ++iter)
    featureDetector->setInt(iter->first, iter->second);
  for (map<string, double>::const_iterator iter = featureDetectorDoubles.begin();
    iter != featureDetectorDoubles.end();
    ++iter)
    featureDetector->setDouble(iter->first, iter->second);
  for (map<string, bool>::const_iterator iter = featureDetectorBools.begin();
    iter != featureDetectorBools.end();
    ++iter)
    featureDetector->setBool(iter->first, iter->second);
}

void InitDescriptorExtractor()
{
  descriptorExtractor = DescriptorExtractor::create(featureType);
}

void Init()
{
  InitFeatureDetector();
  InitDescriptorExtractor();
}

void CorrespondStereo(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame);

int main( int argc, char** argv )
{
  Init();

  string filenameFormat = "/testdata/reika/input/reika.01/%s%04d.avi";
  int iEpisode = 1;
  
  VideoCapture lcap(format(filenameFormat.c_str(), "left", iEpisode)), rcap(format(filenameFormat.c_str(), "right", iEpisode));
  int nFrames = cvRound(lcap.get(CV_CAP_PROP_FRAME_COUNT));
  int w = cvRound(lcap.get(CV_CAP_PROP_FRAME_WIDTH));
  int h = cvRound(lcap.get(CV_CAP_PROP_FRAME_HEIGHT));
  Mat lbgr;
  Mat rbgr;
  Ptr<HCoords> hcoords = new HCoords(w,h);
  KeyPointDescriptorRuler ruler;
  double rootRadius = 5.0;
  double minRadius = 0.15;
  Ptr<CNType> coverNet = new CNType(&ruler, rootRadius, minRadius);
  Ptr<SimpleFrame> lFrame = NULL, rFrame = NULL;
  vector<Ptr<SimpleFrame> > lastFrames;
  lastFrames.reserve(10);
  for (int iFrame = 0; iFrame < nFrames; ++iFrame)
  {
    char c = 0;
    if (!lcap.read(lbgr) || !rcap.read(rbgr))
      return 0;
    lFrame = new SimpleFrame(lbgr, hcoords, iFrame);
    rFrame = new SimpleFrame(rbgr, hcoords, iFrame);
    lFrame->preprocess(featureDetector, descriptorExtractor);
    rFrame->preprocess(featureDetector, descriptorExtractor);
    coverNet = new CNType(&ruler, rootRadius, minRadius);
    InitCoverNet(coverNet, lFrame);
    CorrespondStereo(coverNet, lFrame, rFrame);
    c = cvWaitKey(1);
    if (c == 27)
    {
      return 0;
    }
  }
  return 0;
}

void reproject(Mat todraw, Mat pt3d, Mat cm1, Mat cm2)
{
  Mat tmp = cm1 * pt3d;
  tmp /= tmp.at<double>(0,2);
  Point pt(tmp.at<double>(0,0), tmp.at<double>(0,1));
  circle(todraw, pt, 3, Scalar(0, 255, 0));
  tmp = cm2 * pt3d;
  tmp /= tmp.at<double>(0,2);
  pt = Point(tmp.at<double>(0,0), tmp.at<double>(0,1));
  pt.x += todraw.cols / 2;
  circle(todraw, pt, 3, Scalar(0, 255, 0));
}

void CorrespondStereo(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame)
{
  Mat cm1;//left camera matrix
  Mat cm2;//right camera matrix

  Size sz = lFrame->src.size();
  cv::Mat todraw = Mat::zeros(Size(sz.width * 2, sz.height), CV_8UC3);
  lFrame->src.copyTo(todraw(Rect(Point(0,0), sz)));
  rFrame->src.copyTo(todraw(Rect(Point(sz.width,0), sz)));
  for (unsigned int i = 0; i < lFrame->kps.size(); ++i)
  {
    cv::circle(todraw, lFrame->kps[i].pt, 1, cv::Scalar(255,0,0), 2);
  }
  for (unsigned int i = 0; i < rFrame->kps.size(); ++i)
  {
    Point pt = rFrame->kps[i].pt;
    pt.x += lFrame->src.cols;
    cv::circle(todraw, pt, 1, cv::Scalar(255,0,0), 2);
    double dist = 0.2;
    int iSph = coverNet->findNearestSphere(make_pair(rFrame.obj, i), dist);
    if (iSph != -1)
    {
      SimpleFrame *tmp = coverNet->getSphere(iSph).center.first;
      int ikp = coverNet->getSphere(iSph).center.second;
      cv::line(todraw, pt, tmp->kps[ikp].pt, cv::Scalar(0,0,255), 2);

      Point left = pt;
      Point right = tmp->kps[ikp].pt;

      Mat tri_point;
      TriangulatePoint(tri_point, left, right, cm1, cm2);
      reproject(todraw, tri_point, cm1, cm2);
    }
  }
  cv::Mat rsz;
  resize(todraw, rsz, Size(), 1400.0 / todraw.cols, 1400.0 / todraw.cols);
  imshow("cvnet", rsz);
  coverNet->reportStatistics();
  cout << endl << endl;
  return;
}