#ifndef __DUMBBELLS_H
#define __DUMBBELLS_H
#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include "simpleframe.h"

#include <vector>




//функции вызова эксперимента по нахождению точек схода методом "гантелей"
void MonoCorrespondDumbbells(cv::Ptr<cv::FeatureDetector> featureDetector,
  cv::Ptr<cv::DescriptorExtractor> descriptorExtractor);

struct Dumbbell
{
  SimpleFrame *curFrame;
  SimpleFrame *pivotFrame;
  std::vector<std::pair<unsigned int, unsigned int>> &matches; // pairs: <curFrame fp index, pivotFrame fp index>
  std::pair<unsigned int, unsigned int> heads; //indices of dumbbell heads entries in matches array

  cv::Point3d rotationVector;

  Dumbbell(std::vector<std::pair<unsigned int, unsigned int>> &matches_)
    :matches(matches_) {}

  Dumbbell(const Dumbbell &cpy)
    :matches(cpy.matches)
    , curFrame(cpy.curFrame)
    , pivotFrame(cpy.pivotFrame)
    , heads(cpy.heads)
    , rotationVector(cpy.rotationVector)
  {}
  
  Dumbbell & operator=(const Dumbbell &rhs)
  {
    matches = rhs.matches;
    curFrame = rhs.curFrame;
    pivotFrame = rhs.pivotFrame;
    heads = rhs.heads;
    rotationVector = rhs.rotationVector;
    return *this;
  }


  cv::Point3d & ComputeRotationVector();
};

#endif //__DUMBBELLS_H