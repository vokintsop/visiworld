#ifndef __CORRESPOND_H
#define __CORRESPOND_H
#pragma once

#include <opencv2/core/core.hpp>

#include "rulers.h"
#include "simpleframe.h"
#include "pointmatch.h"


void CorrespondStereo(
  KeyPointDescriptorRuler *ruler, 
  cv::Ptr<SimpleFrame> &lFrame, 
  cv::Ptr<SimpleFrame> &rFrame, 
  PointMatches &pm);

void CorrespondStereoWithCNet(
  KeyPointDescriptorRuler *ruler,
  cv::Ptr<SimpleFrame> &lFrame, 
  cv::Ptr<SimpleFrame> &rFrame, 
  PointMatches &pm, 
  cv::Mat &todraw/* = cv::Mat()*/); // never bind temporary value by non-const reference!

void CorrespondStereoWithMinimalDist(
  KeyPointDescriptorRuler *ruler,
  cv::Ptr<SimpleFrame> &lFrame, 
  cv::Ptr<SimpleFrame> &rFrame, 
  PointMatches &pm,
  cv::Mat &todraw/* = cv::Mat()*/); // never bind temporary value by non-const reference!

void CorrespondStereoWithFlann(
  KeyPointDescriptorRuler *ruler,
  cv::Ptr<SimpleFrame> &lFrame,
  cv::Ptr<SimpleFrame> &rFrame, 
  PointMatches &pm, 
  cv::Mat &todraw/* = cv::Mat()*/); // never bind temporary value by non-const reference!

#endif //__CORRESPOND_H
