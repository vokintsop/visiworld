#include "monocor.h"
#include "init.h"

#include <ocvutils/hcoords.h> 
#include <cover_net/cover_net.h>

#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void TryCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &curFrame);
//функции вызова эксперимента по монокул€рной временной корреспонденции особых 
//точек с помощью CoverNet
void MonoCorrespondTime(Ptr<FeatureDetector> featureDetector,
  Ptr<DescriptorExtractor> descriptorExtractor)
{
  string ifilename = "/testdata/roadvideo/input/roadvideo.02/roadvideo.02.0001.avi";

  VideoCapture cap(ifilename);
  int nFrames = cvRound(cap.get(CV_CAP_PROP_FRAME_COUNT));
  int w = cvRound(cap.get(CV_CAP_PROP_FRAME_WIDTH));
  int h = cvRound(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  Mat bgr;
  Ptr<HCoords> hcoords(new HCoords(w,h));
  KeyPointDescriptorRuler ruler;
  double rootRadius = 5.0;
  double minRadius = 0.15;
  Ptr<CNType> coverNet(new CNType(&ruler, rootRadius, minRadius));
  Ptr<SimpleFrame> pivotFrame, curFrame;
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
#if CV_MAJOR_VERSION > 2
      curFrame.reset(new SimpleFrame(bgr, hcoords, iFrame));
#else
      curFrame = new SimpleFrame(bgr, hcoords, iFrame);
#endif
      lastFrames.push_back(curFrame);
      curFrame->preprocess(featureDetector, descriptorExtractor);
      if (coverNetFlush == 0)
      {
#if CV_MAJOR_VERSION > 2
        coverNet.reset(new CNType(&ruler, rootRadius, minRadius));
#else
        coverNet = new CNType(&ruler, rootRadius, minRadius);
#endif
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
#if CV_MAJOR_VERSION > 2
    int iSph = coverNet->findNearestSphere(make_pair(curFrame.get(), i), dist);
#else
    int iSph = coverNet->findNearestSphere(make_pair(curFrame.obj, i), dist);
#endif
    
    if (iSph == -1) //no newarest point found
#if CV_MAJOR_VERSION > 2
      iSph = coverNet->insert(make_pair(curFrame.get(), i)); //creating new coverNet Sphere
#else
      iSph = coverNet->insert(make_pair(curFrame.obj, i)); //creating new coverNet Sphere
#endif
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
