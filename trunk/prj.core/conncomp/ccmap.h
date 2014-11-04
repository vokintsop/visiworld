#ifndef __CCMAP_H
#define __CCMAP_H

// ccmap.h -- ����� ��������� ���������
#include <vector>
#include <opencv2/core/core.hpp>

#include "ccdata.h"

class CCMap
{
public:
  cv::Mat1i labels; // ����� ��������� ���������, [2...labels.size()-1] ==> cc[]
  std::vector< CCData > cc; // ���������� � �����������
  CCMap(){};
  bool compute( cv::Mat1b& from );
  void draw( cv::Mat& to, cv::Scalar& color, int thickness_not_filtered, int thickness_filtered );
};

#endif // __CCMAP_H
