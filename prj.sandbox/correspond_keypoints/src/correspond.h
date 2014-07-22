#ifndef __CORRESPOND_H
#define __CORRESPOND_H
#pragma once

#include <opencv2/core/core.hpp>

#include "rulers.h"
#include "simpleframe.h"


void CorrespondStereo(
  KeyPointDescriptorRuler *ruler, 
  cv::Ptr<SimpleFrame> &lFrame, 
  cv::Ptr<SimpleFrame> &rFrame, 
  cv::FileStorage &outFS);

void CorrespondStereoWithCNet(
  KeyPointDescriptorRuler *ruler,
  cv::Ptr<SimpleFrame> &lFrame, 
  cv::Ptr<SimpleFrame> &rFrame, 
  cv::FileStorage &outFS, 
  cv::Mat &todraw = cv::Mat());

void CorrespondStereoWithMinimalDist(
  KeyPointDescriptorRuler *ruler,
  cv::Ptr<SimpleFrame> &lFrame, 
  cv::Ptr<SimpleFrame> &rFrame, 
  cv::FileStorage &outFS,
  cv::Mat &todraw = cv::Mat());

void CorrespondStereoWithFlann(
  KeyPointDescriptorRuler *ruler,
  cv::Ptr<SimpleFrame> &lFrame,
  cv::Ptr<SimpleFrame> &rFrame, 
  cv::FileStorage &outFS, 
  cv::Mat &todraw = cv::Mat());

#endif //__CORRESPOND_H