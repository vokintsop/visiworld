#ifndef __MONOCOR_H
#define __MONOCOR_H
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

//������� ������ ������������ �� ������������ ��������� ��������������� ������ 
//����� � ������� CoverNet

void MonoCorrespondTime(cv::Ptr<cv::FeatureDetector> featureDetector,
  cv::Ptr<cv::DescriptorExtractor> descriptorExtractor);
#endif //__MONOCOR_H