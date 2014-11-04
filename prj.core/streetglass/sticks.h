#ifndef __STICKS_H
#define __STICKS_H

struct Stick
{
  cv::Point a, b;
};

bool computeVerticalSticsAsMat1b( cv::Mat& img, cv::Mat1b& vsticks );

#endif // __STICKS_H