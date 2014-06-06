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
#include "calibration.h"



enum Key {
  kEscape =27,
  kEnter =13,
  kLeftArrow =2424832,
  kRightArrow =2555904,
  kPageUp =2162688,
  kPageDown =2228224,
  kSpace =32,
  kTab =9,
  kGreyPlus =43,
  kGreyMinus =45,
  kNoKeyPressed =-1  // after positive delay no key pressed -- process next image
};



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

bool OpenVideoCaptures(int argc, char** argv, VideoCapture &capL, VideoCapture &capR);
bool ProcessCapturesData(VideoCapture &capL, VideoCapture &capR);

int main( int argc, char** argv )
{
  Init();
  VideoCapture capL, capR;
  OpenVideoCaptures(argc, argv, capL, capR);
  ProcessCapturesData(capL, capR);

  return 0;
}

bool ProcessCapturesData(VideoCapture &capL, VideoCapture &capR)
{
  char c = cvWaitKey(1);
  while (c != Key::kEscape)
  { 
    Mat left, right;
    ReadLeftAndRightFrames(capL, capR, left, right);
    Ptr<SimpleFrame> lFrame = new SimpleFrame(left), rFrame = new SimpleFrame(right);
    lFrame->preprocess(featureDetector, descriptorExtractor);
    rFrame->preprocess(featureDetector, descriptorExtractor);
    vector<pair<int, int> > pointsCorrespondence;
    FindCorrespondingPoints(lFrame, rFrame, pointsCorrespondence);
    DrawCorrPoints(lFrame, rFrame, pointsCorrespondence);
    c = cvWaitKey(1);
  }
  return true;
}



bool OpenVideoCaptures(int argc, char** argv, VideoCapture &capL, VideoCapture &capR)
{
  if (argc == 2) //from file
    return OpenCaptures(argv[1], capL, capR);
  if (argc == 3)
    return OpenCaptures(atoi(argv[1]), atoi(argv[2]), capL, capR);
  
  return false;
}


void CorrespondStereo(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame)
{
  Mat cm1 = (Mat_<double>(3,4) << 802.9362, 0,        640.0000, 0.5 * 923.3766, 
                                  0,        802.9362, 360.0000, 0, 
                                  0,        0,        1.0000,   0);//left camera matrix

  Mat cm2 = (Mat_<double>(3,4) << 802.9362, 0,        640.0000, -0.5 * 923.3766,//-1200,//
                                  0,        802.9362, 360.0000, 0,
                                  0,        0,        1.0000,   0);//right camera matrix 
  cout << cm1 << endl << cm2 << endl << endl;
}