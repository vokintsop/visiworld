#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "simpleframe.h"
#include "keypointruler.h"

#include <ocvutils/hcoords.h> 
#include <cover_net/cover_net.h>




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

typedef CoverNet<std::pair<SimpleFrame *, unsigned int>, KeyPointDescriptorRuler> CNType;

void InitCoverNet(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &pivotFrame);
void TryCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &curFrame);
void TryCorrespondStereo(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame);

void MonoCorrespondTime();
void MonoCorrespondDumbbells();
void StereoCorrespond();

int main( int argc, char** argv )
{
  Init();
 // MonoCorrespondTime();
  MonoCorrespondDumbbells();
 // StereoCorrespond();
  return 0;
}

void MonoCorrespondTime()
{
  string ifilename = "/testdata/roadvideo/input/roadvideo.02/roadvideo.02.0001.avi";

  VideoCapture cap(ifilename);
  int nFrames = cvRound(cap.get(CV_CAP_PROP_FRAME_COUNT));
  int w = cvRound(cap.get(CV_CAP_PROP_FRAME_WIDTH));
  int h = cvRound(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  Mat bgr;
  Ptr<HCoords> hcoords = new HCoords(w,h);
  KeyPointDescriptorRuler ruler;
  double rootRadius = 5.0;
  double minRadius = 0.15;
  Ptr<CNType> coverNet = new CNType(&ruler, rootRadius, minRadius);
  Ptr<SimpleFrame> pivotFrame = NULL, curFrame = NULL;
  vector<Ptr<SimpleFrame> > lastFrames;
  lastFrames.reserve(10);
  int coverNetFlush = 0;
  for (int iFrame = 0; iFrame < nFrames; ++iFrame)
  {
    char c = 0;
    if (!cap.read(bgr))
      return;
    if (!(iFrame % 4))
    {
      curFrame = new SimpleFrame(bgr, hcoords, iFrame);
      lastFrames.push_back(curFrame);
      curFrame->preprocess(featureDetector, descriptorExtractor);
      if (coverNetFlush == 0)
      {
        coverNet = new CNType(&ruler, rootRadius, minRadius);
        pivotFrame = curFrame;
        InitCoverNet(coverNet, pivotFrame);
        lastFrames.clear();
        lastFrames.reserve(10);
        coverNetFlush = 0;
      }
      //curFrame->draw();
      TryCorrespond(coverNet, curFrame);
      c = cvWaitKey(2);
      ++coverNetFlush;
      coverNetFlush %= 10;
    }
    if (c == 27)
    {
      return;
    }
  }

}



void TryCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &curFrame)
{
  cv::Mat todraw = curFrame->src.clone();
  
  for (unsigned int i = 0; i < curFrame->kps.size(); ++i)
  {
    cv::circle(todraw, curFrame->kps[i].pt, 1, cv::Scalar(255,0,0), 2);
    double dist = 0.2;
    int iSph = coverNet->findNearestSphere(make_pair(curFrame, i), dist);
    if (iSph == -1) //no newarest point found
      iSph = coverNet->insert(make_pair(curFrame, i)); //creating new coverNet Sphere
    if (iSph != -1)
    {
      SimpleFrame *tmp = coverNet->getSphere(iSph).center.first;
      int ikp = coverNet->getSphere(iSph).center.second;
      cv::circle(todraw, tmp->kps[ikp].pt, 1, cv::Scalar(0,0,255), 1);  
      cv::line(todraw, curFrame->kps[i].pt, tmp->kps[ikp].pt, cv::Scalar(0,0,255), 1);
    }
  }
  cv::Mat rsz = todraw;
  //resize(todraw, rsz, Size(), 0.5, 0.5);
  imshow("cvnet", rsz);
  coverNet->reportStatistics();
  cout << endl << endl;
  
  return;
}

void StereoCorrespond()
{
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
      return;
    lFrame = new SimpleFrame(lbgr, hcoords, iFrame);
    rFrame = new SimpleFrame(rbgr, hcoords, iFrame);
    lFrame->preprocess(featureDetector, descriptorExtractor);
    rFrame->preprocess(featureDetector, descriptorExtractor);
    coverNet = new CNType(&ruler, rootRadius, minRadius);
    InitCoverNet(coverNet, lFrame);
    TryCorrespondStereo(coverNet, lFrame, rFrame);
    c = cvWaitKey(1);
    if (c == 27)
    {
      return;
    }
  }

}


void TryCorrespondStereo(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame)
{
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
    int iSph = coverNet->findNearestSphere(make_pair(rFrame, i), dist);
    /*
    if (iSph == -1) //no newarest point found
      iSph = coverNet->insert(make_pair(rFrame, i)); //creating new coverNet Sphere
      */
    if (iSph != -1)
    {
      SimpleFrame *tmp = coverNet->getSphere(iSph).center.first;
      int ikp = coverNet->getSphere(iSph).center.second;
      cv::line(todraw, pt, tmp->kps[ikp].pt, cv::Scalar(0,0,255), 2);
    }
  }
  cv::Mat rsz;
  resize(todraw, rsz, Size(), 1400.0 / todraw.cols, 1400.0 / todraw.cols);
  imshow("cvnet", rsz);
  coverNet->reportStatistics();
  cout << endl << endl;
  return;
}

void InitCoverNet(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &pivotFrame)
{
  if (!coverNet || !pivotFrame)
    return;
  for (unsigned int i = 0; i < pivotFrame->kps.size(); ++i)
  {
    coverNet->insert(make_pair(pivotFrame, i));
  }
}

void DumbbellCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &pivotFrame, Ptr<SimpleFrame> &curFrame);

void MonoCorrespondDumbbells()
{
  string ifilename = "/testdata/roadvideo/input/roadvideo.02/roadvideo.02.0001.avi";
  VideoCapture cap(ifilename);
  int nFrames = cvRound(cap.get(CV_CAP_PROP_FRAME_COUNT));
  int w = cvRound(cap.get(CV_CAP_PROP_FRAME_WIDTH));
  int h = cvRound(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  Mat bgr;
  Ptr<HCoords> hcoords = new HCoords(w,h);
  KeyPointDescriptorRuler ruler;
  double rootRadius = 5.0;
  double minRadius = 0.15;
  Ptr<CNType> coverNet = new CNType(&ruler, rootRadius, minRadius);
  Ptr<SimpleFrame> pivotFrame = NULL, curFrame = NULL;
  int coverNetFlush = 0;
  for (int iFrame = 0; iFrame < nFrames; ++iFrame)
  {
    char c = 0;
    if (!cap.read(bgr))
      return;
    if (!(iFrame % 4))
    {
      curFrame = new SimpleFrame(bgr, hcoords, iFrame);
      curFrame->preprocess(featureDetector, descriptorExtractor);
      if (coverNetFlush == 0)
      {
        coverNet = new CNType(&ruler, rootRadius, minRadius);
        pivotFrame = curFrame;
        InitCoverNet(coverNet, pivotFrame);
        coverNetFlush = 0;
      }
      DumbbellCorrespond(coverNet, pivotFrame, curFrame);
      curFrame->draw();
      c = cvWaitKey(0);
      ++coverNetFlush;
      coverNetFlush %= 10;
    }
    if (c == 27)
    {
      return;
    }
  }
}

struct Dumbbell
{
  SimpleFrame *curFrame;
  SimpleFrame *pivotFrame;
  vector<pair<unsigned int, unsigned int>> &matches; // pairs: <curFrame fp index, pivotFrame fp index>
  std::pair<unsigned int, unsigned int> heads; //indices of dumbbell heads entries in matches array

  Point3d rotationVector;

  Dumbbell(vector<pair<unsigned int, unsigned int>> &matches_)
    :matches(matches_) {}

  Point3d & ComputeRotationVector()
  {
    HPoint3d P1 = pivotFrame->sphere_points[matches[heads.first].second];
    HPoint3d P2 = pivotFrame->sphere_points[matches[heads.second].second];
    HPoint3d Q1 = curFrame->sphere_points[matches[heads.first].first];
    HPoint3d Q2 = curFrame->sphere_points[matches[heads.second].first];

    Point3d d1 = Q1 - P1;
    Point3d d2 = Q2 - P2;

    double cosd1d2 = d1.dot(d2) / sqrt((d1.dot(d1) * d2.dot(d2)));
    Point3d n = (d1).cross(d2);
    if (cosd1d2 >= 0.99)
    {
      cout << "dumbbell points lie on 1 meridian" << endl;
      n = P1.cross(P2).cross(Q1.cross(Q2));
    }
    double norm_n = norm(n);
    if (norm_n != 0)
    {
      n.x /= norm_n;
      n.y /= norm_n;
      n.z /= norm_n;
    }
    rotationVector = n;
    return rotationVector;
  }
};




void DumbbellCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &pivotFrame, Ptr<SimpleFrame> &curFrame)
{  
  if (pivotFrame == curFrame)
    return;
  //find matches
  vector<pair<unsigned int, unsigned int>> matches;
  for (unsigned int i = 0; i < curFrame->kps.size(); ++i)
  {
    double dist = 0.2;
    int iSph = coverNet->findNearestSphere(make_pair(curFrame, i), dist);
    if (iSph != -1)
    {
      int ikp = coverNet->getSphere(iSph).center.second;
      matches.push_back(make_pair(i, coverNet->getSphere(iSph).center.second));
    }
  }
  
  //for all matches make a Dumbbell
  vector<Dumbbell> dumbbells;
  for (int i = 0; i < matches.size(); ++i)
    for (int j = i + 1; j < matches.size(); ++j)
    {
      Dumbbell d(matches);
      d.curFrame = curFrame;
      d.pivotFrame = pivotFrame;
      d.heads.first = i;
      d.heads.second = j;
      dumbbells.push_back(d);
    }

  UnitSphereAnglesRuler angRuler;
  CoverNet<Point3d &, UnitSphereAnglesRuler> angCNet(&angRuler, CV_PI, CV_PI/180);
  for (int i = 0; i < dumbbells.size(); ++i)
  {
    angCNet.insert(dumbbells[i].ComputeRotationVector());
  }

  angCNet.reportStatistics();
  return;
}




















