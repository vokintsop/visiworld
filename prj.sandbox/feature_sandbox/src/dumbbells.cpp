#include "dumbbells.h"
#include "init.h"

#include <ocvutils/hcoords.h> 
#include <cover_net/cover_net.h>

#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


  cv::Point3d & Dumbbell::ComputeRotationVector()
  {
    HPoint3d P1 = pivotFrame->sphere_points[matches[heads.first].second];
    HPoint3d P2 = pivotFrame->sphere_points[matches[heads.second].second];
    HPoint3d Q1 = curFrame->sphere_points[matches[heads.first].first];
    HPoint3d Q2 = curFrame->sphere_points[matches[heads.second].first];

    if (abs(norm(P2 - P1) - norm(Q2 - Q1)) >= .05)
    {
      cout << "Pair should be rejected as a dumbbell" << endl;
    }

    Point3d d1 = Q1 - P1;
    Point3d d2 = Q2 - P2;

    double cosd1d2 = d1.dot(d2) / sqrt((d1.dot(d1) * d2.dot(d2)));
    Point3d n = (d1).cross(d2);
    if (cosd1d2 >= 0.99)
    {
     // cout << "dumbbell points lie on 1 meridian" << endl;
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

void DumbbellCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &pivotFrame, Ptr<SimpleFrame> &curFrame)
{  
  if (pivotFrame == curFrame)
    return;
  //find matches
  vector<pair<unsigned int, unsigned int>> matches;
  for (unsigned int i = 0; i < curFrame->kps.size(); ++i)
  {
    double dist = 0.2;
    int iSph = coverNet->findNearestSphere(make_pair(curFrame.obj, i), dist);
    if (iSph != -1)
    {
      int ikp = coverNet->getSphere(iSph).center.second;
      matches.push_back(make_pair(i, coverNet->getSphere(iSph).center.second));
    }
  }
  
  //for all matches make a Dumbbell
  vector<Dumbbell> dumbbells;
  for (unsigned int i = 0; i < matches.size(); ++i)
    for (unsigned int j = i + 1; j < matches.size(); ++j)
    {
      Dumbbell d(matches);
      d.curFrame = curFrame;
      d.pivotFrame = pivotFrame;
      d.heads.first = i;
      d.heads.second = j;
      dumbbells.push_back(d);
    }

  UnitSphereAnglesRuler angRuler;
  CoverNet<Point3d *, UnitSphereAnglesRuler> angCNet(&angRuler, CV_PI, CV_PI/180);
  for (unsigned int i = 0; i < dumbbells.size(); ++i)
  {
    angCNet.insert(&dumbbells[i].ComputeRotationVector());
  }

  angCNet.reportStatistics();
  return;
}

void MonoCorrespondDumbbells(Ptr<FeatureDetector> featureDetector,
  Ptr<DescriptorExtractor> descriptorExtractor)
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

