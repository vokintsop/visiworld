#ifndef KITTI_H__
#define KITTI_H__
#pragma once

#include <string>
#include <vector>

bool readTimeStamps(const std::string &fname, std::vector<double> &timestamps);

#endif //KITTI_H__