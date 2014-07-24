#ifndef __POINTMATCH_H
#define __POINTMATCH_H
#pragma once

#include <vector>
#include <opencv2/core/core.hpp>

typedef std::vector<std::pair<cv::Point, cv::Point> > PointMatches;
typedef std::vector<PointMatches> PointMatchStorage; // [frame][match]

bool ReadPointMatchStorage(cv::FileNode &root, PointMatchStorage &pms);
bool WritePointMatchStorage(cv::FileStorage &fs, PointMatchStorage &pms);

#endif //__POINTMATCH_H