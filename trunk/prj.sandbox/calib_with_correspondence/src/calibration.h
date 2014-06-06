#ifndef __CALIBRATION_H
#define __CALIBRATION_H
#pragma once

#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include "simpleframe.h"

typedef std::vector<std::pair<int, int> > PointCorType;

bool OpenCaptures(const std::string &fname_pattern, cv::VideoCapture &capL, cv::VideoCapture &capR);
bool OpenCaptures(int lIndex, int rIndex, cv::VideoCapture &capL, cv::VideoCapture &capR);

bool ReadLeftAndRightFrames(cv::VideoCapture &capL, cv::VideoCapture &capR, cv::Mat &left, cv::Mat &right);

bool FindCorrespondingPoints(cv::Ptr<SimpleFrame> &lFrame, cv::Ptr<SimpleFrame> &rFrame, PointCorType &pointsCorrespondence);
bool DrawCorrPoints(cv::Ptr<SimpleFrame> &lFrame, cv::Ptr<SimpleFrame> &rFrame, PointCorType &pointsCorrespondence);


#endif //__CALIBRATION_H