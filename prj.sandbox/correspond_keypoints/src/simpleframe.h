#ifndef __SIMPLEFRAME_H
#define __SIMPLEFRAME_H
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <vector>

/*
Простенький класс SimpleFrame - аналог MyFrame в player. он будет сохранять кадр
видеопотока, результаты детекции FP, результаты обработки.
*/
class SimpleFrame
{
public:
  SimpleFrame(const cv::Mat &bgr = cv::Mat(), int iFrame_ = 0)
    :src(bgr.clone()), iFrame(iFrame_), sigmaDescr(0)
  {}

  void preprocess(cv::Ptr<cv::FeatureDetector> detector, 
    cv::Ptr<cv::DescriptorExtractor> extractor);

  cv::Mat src;
  int iFrame;

  std::vector<cv::KeyPoint> kps;
  cv::Mat descriptors;
  cv::Mat meanDescr;
  double sigmaDescr;

  void draw(std::string window_name = "");
private:  
};
#endif //__SIMPLEFRAME_H