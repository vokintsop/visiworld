#ifndef __STEREOCOR_H
#define __STEREOCOR_H
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

//������� ������ ������������ �� ��������������� ������ ����� �� ����������
//� ������� CoverNet

void StereoCorrespondTime(cv::Ptr<cv::FeatureDetector> featureDetector,
  cv::Ptr<cv::DescriptorExtractor> descriptorExtractor);
#endif //__STEREOCOR_H