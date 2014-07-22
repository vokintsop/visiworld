#ifndef __INIT_H_123
#define __INIT_H_123
#pragma once

#include <opencv2/core/core.hpp>
#include <cover_net/cover_net.h>

#include "simpleframe.h"
#include "rulers.h"



typedef CoverNet<std::pair<SimpleFrame *, unsigned int>, KeyPointDescriptorRuler> CNType;

inline void InitCoverNet(cv::Ptr<CNType> &coverNet, cv::Ptr<SimpleFrame> &pivotFrame)
{
  if (!coverNet || !pivotFrame)
    return;
  for (unsigned int i = 0; i < pivotFrame->kps.size(); ++i)
  {
    coverNet->insert(std::make_pair(pivotFrame.obj, i));
  }
}


#endif //__INIT_H