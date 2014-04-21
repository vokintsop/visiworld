#ifndef __SIMPLEFRAME_H
#define __SIMPLEFRAME_H
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include <ocvutils/hcoords.h>

/*
����������� ����� SimpleFrame - ������ MyFrame � player. �� ����� ��������� ����
�����������, ���������� �������� FP, ���������� ���������.
*/
class SimpleFrame
{
public:
  SimpleFrame(const cv::Mat &bgr = cv::Mat(), 
    cv::Ptr<HCoords> &hc_ = cv::Ptr<HCoords>(NULL),
    int iFrame_ = 0)
    :src(bgr.clone()), iFrame(iFrame_), hc(hc_) 
  {
    if (!hc)
      hc = new HCoords(src.cols, src.rows);
  }

  void preprocess(cv::Ptr<cv::FeatureDetector> detector, 
    cv::Ptr<cv::DescriptorExtractor> extractor);

  cv::Mat src;
  int iFrame;

  std::vector<cv::KeyPoint> kps;
  cv::Mat descriptors;
  std::vector<HPoint3d> sphere_points;

  void draw()
  {
    cv::Mat draw_mat = src.clone();
    for (unsigned int i = 0; i < kps.size(); ++i)
    {
      cv::circle(draw_mat, kps[i].pt, 2, cv::Scalar(255,0,0), 2);
      cv::circle(draw_mat, kps[i].pt, kps[i].size, cv::Scalar(255,0,0));
    }

    cv::Mat draw_mat_resized;
    cv::resize(draw_mat, draw_mat_resized, cv::Size(), 0.5, 0.5);
    imshow("detected_points", draw_mat_resized);
  }

private:  
  void project()
  {
    for (unsigned int i = 0; i < kps.size(); ++i)
    {
      cv::Point tmppt(cvRound(kps[i].pt.x), cvRound(kps[i].pt.y));
      sphere_points.push_back(hc->point2hpoint(tmppt));
    }
  }

  cv::Ptr<HCoords> hc;
};
#endif //__SIMPLEFRAME_H