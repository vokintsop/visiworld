#include "stereocor.h"
#include "init.h"

#include <ocvutils/hcoords.h> 
#include <cover_net/cover_net.h>

#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void TryCorrespondStereo(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &lFrame, Ptr<SimpleFrame> &rFrame);

void StereoCorrespond(Ptr<FeatureDetector> featureDetector,
  Ptr<DescriptorExtractor> descriptorExtractor)
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
    int iSph = coverNet->findNearestSphere(make_pair(rFrame.obj, i), dist);
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