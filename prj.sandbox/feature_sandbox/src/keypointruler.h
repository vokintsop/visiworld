#ifndef __KEYPOINTRULER_H
#define __KEYPOINTRULER_H
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <cover_net/cover_net.h>
#include "simpleframe.h"

class KeyPointDescriptorRuler{
public:
  //std::vector<cv::KeyPoint> &kps; // keyPoints array reference
  //cv::Mat &descriptros; //Mat of descriptors reference. It's size is n x kps.size(), where n is descriptor dimension
 
  //KeyPointDescriptorRuler( std::vector< cv::KeyPoint > &kps_, cv::Mat &descriptors_): 
  //  kps(kps_), descriptros(descriptors_) {};
  

  KeyPointDescriptorRuler() {}

  double computeDistance(  const std::pair<SimpleFrame *, unsigned int> &pt1, const std::pair<SimpleFrame *, unsigned int> &pt2 )
  {
    //double alpha = 2.0 / 2202.0; //mono 1920
    double alpha = 2.0 / 1470.0; //stereo 1280
    cv::Mat desc1 = pt1.first->descriptors.row(pt1.second);
    cv::Mat desc2 = pt2.first->descriptors.row(pt2.second);
    cv::Point2f kp1 = pt1.first->kps[pt1.second].pt;
    cv::Point2f kp2 = pt2.first->kps[pt2.second].pt;
    return alpha * pointDist(kp1, kp2) + cv::norm(desc1 - desc2);
  }

private:
  static double pointDist(const cv::Point2f &pt1, const cv::Point2f &pt2)
  {
    double res = sqrt(pow(pt1.x - pt2.x, 2.0) + pow(pt1.y - pt2.y, 2.0));
    //if (res <= 100)//mono 1920
    if (res <= 200) //stereo 1280
      return 0;
    return res;
  }
};

#endif //__KEYPOINTRULER_H