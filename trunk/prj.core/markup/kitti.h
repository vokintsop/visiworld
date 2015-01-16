#ifndef KITTI_H__
#define KITTI_H__
#pragma once

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

bool readTimeStamps(const std::string &fname, std::vector<double> &timestamps);

//класс дл€ чтени€ последовательности пнг-китти 
class KittiCapture
{
public:
  KittiCapture(const std::string &sequence_path = "");
  bool open(const std::string &sequence_path);
  bool is_opened() const;
  void release();
  bool read(cv::Mat &img);

  double get(int propid) const;
  bool set(int propid, double propval);

  cv::Size getImgSize() const;
  int getFramesNum() const;
  double getFrameMsec(int frameid) const;
  double getCurMsec() const;
  int getCurFrameNum() const;
  bool setCurFrame(int frameid);
  bool getFrame(int frameid, cv::Mat &img) const;
  double getFps() const;

private:
  std::string seq_path;
  std::vector<double> timestamps;
  int curframe;
  double fps;
  cv::Size imgSize;
};
#endif //KITTI_H__