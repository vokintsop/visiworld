#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "simpleframe.h"
#include "keypointruler.h"

#include <ocvutils\hcoords.h> 
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

void TryCorrespond(Ptr<CNType> &curFrame, Ptr<SimpleFrame> &prevFrame);

int main( int argc, char** argv )
{
  Init();
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
  Ptr<SimpleFrame> prevFrame = NULL, curFrame = NULL;
  vector<Ptr<SimpleFrame> > lastFrames;
  lastFrames.reserve(10);
  int coverNetFlush = 0;
  for (int iFrame = 0; iFrame < nFrames; ++iFrame)
  {
    char c = 0;
    if (!cap.read(bgr))
      return 0;
    if (!(iFrame % 4))
    {
      curFrame = new SimpleFrame(bgr, hcoords, iFrame);
      lastFrames.push_back(curFrame);
      curFrame->preprocess(featureDetector, descriptorExtractor);
      //curFrame->draw();
      TryCorrespond(coverNet, curFrame);
      c = cvWaitKey(0);
      prevFrame = curFrame;
      ++coverNetFlush;
      if (coverNetFlush == 10)
      {
        coverNet = new CNType(&ruler, rootRadius, minRadius);
        lastFrames.clear();
        lastFrames.reserve(10);
        coverNetFlush = 0;
      }
    }
    if (c == 27)
    {
      return 0;
    }
  }



  return 0;
}

void TryCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &curFrame)
{
  cv::Mat todraw = curFrame->src.clone();
  for (unsigned int i = 0; i < curFrame->kps.size(); ++i)
  {
    cv::circle(todraw, curFrame->kps[i].pt, 1, cv::Scalar(255,0,0), 2);
    int iSph = coverNet->insert(make_pair(curFrame, i));
    SimpleFrame *tmp = coverNet->getSphere(iSph).center.first;
    int ikp = coverNet->getSphere(iSph).center.second;
    cv::circle(todraw, tmp->kps[ikp].pt, 1, cv::Scalar(0,0,255), 1);
    cv::line(todraw, curFrame->kps[i].pt, tmp->kps[ikp].pt, cv::Scalar(0,0,255), 1);
  }
  cv::Mat rsz = todraw;
  //resize(todraw, rsz, Size(), 0.5, 0.5);
  imshow("cvnet", rsz);
  coverNet->reportStatistics();
  cout << endl << endl;
  return;
}