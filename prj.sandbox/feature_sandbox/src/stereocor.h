#ifndef __STEREOCOR_H
#define __STEREOCOR_H
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

//функции вызова эксперимента по корреспонденции особых точек на стереопаре
//с помощью CoverNet

void StereoCorrespondTime(cv::Ptr<cv::FeatureDetector> featureDetector,
  cv::Ptr<cv::DescriptorExtractor> descriptorExtractor);
#endif //__STEREOCOR_H