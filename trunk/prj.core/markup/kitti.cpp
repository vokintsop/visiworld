#include "kitti.h"
#include "ocvutils/precomp.h"
#include <fstream>
#include <iostream>
#include <iterator>

#include <cstdio>

using namespace std;


bool readTimeStamps(const string &fname, vector<double> &timestamps)
{
  ifstream fin(fname.c_str());
  if (!fin.good())
    return __false(format("\nCan't open timestamp file %s\n", fname.c_str()));
    
  vector<string> timestamps_str;
  while (fin.good())
  {
    string str;
    getline(fin, str);
    if (!str.empty())
      timestamps_str.push_back(str);
  }
  fin.close();

  for (unsigned int i = 0; i < timestamps_str.size(); ++i)
  {
    string p = timestamps_str[i];
    tm time = {0};
    double secs = 0.0;
#ifdef _MSC_VER
    if (sscanf_s(p.c_str(), "%d-%d-%d %d:%d:%lf", &time.tm_year, &time.tm_mon, &time.tm_mday,
      &time.tm_hour, &time.tm_min, &secs) == EOF)
      return __false(format("\nWrong format of timpestamp file: %s", fname));
#else // _MSC_VER
    if (sscanf(p.c_str(), "%d-%d-%d %d:%d:%lf", &time.tm_year, &time.tm_mon, &time.tm_mday,
      &time.tm_hour, &time.tm_min, &secs) == EOF)
      return __false(std::string("\nWrong format of timpestamp file: ") + fname);
#endif // _MSC_VER
    double int_secs = 0.0, dec_secs = 0.0;
    dec_secs = modf(secs, &int_secs);
    time.tm_sec = static_cast<int> (int_secs);
    time.tm_year -= 1900; //так работает функция
    time_t dtime = mktime(&time);
    if (dtime == -1)
      return __false("\nError reading timestamp\n");
    timestamps.push_back(double(mktime(&time)) + dec_secs);
  }
  return true;
}


KittiCapture::KittiCapture(const string &sequence_path)
  : seq_path(sequence_path)
  , curframe(0)
  , fps(.0)
  , imgSize()
{
  if (!seq_path.empty())
    open(seq_path);
}

bool KittiCapture::open(const string &sequence_path)
{
  seq_path = sequence_path;
  if (!readTimeStamps(seq_path + "/timestamps.txt", timestamps))
  {
    seq_path = "";
    return false;
  }

  if (timestamps.empty())
    return __false(format("\nError opening kitti sequence %s", seq_path.c_str()));
  
  double start_time = timestamps[0];
  for (unsigned int i = 0; i < timestamps.size(); ++i)
  {
    timestamps[i] -= start_time;
  }
  
  //now calculate average fps:
  double mean = 0;
  for (unsigned int i = 0; i < timestamps.size() - 1; ++i)
    mean += timestamps[i + 1] - timestamps[i];
  mean /= (timestamps.size() - 1);
    
  if (mean == 0)
    return __false("\nError wrong timestamp file\n");
    
  fps = 1 / mean;

  //нужна ли проверка наличия всех кадров?
  Mat img;
  if (!getFrame(0, img))
    return false;  
  imgSize = img.size();
  return true;
}

bool KittiCapture::read(Mat &img)
{
  if (!getFrame(curframe, img))
    return false;
  ++curframe;
  return true;
}

bool KittiCapture::getFrame(int frameid, Mat &img) const
{
  if (frameid < 0 || frameid >= getFramesNum())
    return false;

  string imgfname = seq_path + format("/data/%010d.png", frameid);
  img = imread(imgfname);
  return !img.empty();
}

bool KittiCapture::is_opened() const
{
  return imgSize.area() != 0;
}

void KittiCapture::release()
{
  seq_path = "";
  timestamps.clear();
  curframe = 0;
  fps = 0;
  imgSize = Size();
}

cv::Size KittiCapture::getImgSize() const
{
  return imgSize;
}

int KittiCapture::getFramesNum() const
{
  return timestamps.size();
}

double KittiCapture::getFrameMsec(int frameid) const
{
  if (frameid > 0 && frameid >= static_cast<int> (timestamps.size()))
    frameid = timestamps.size() - 1;
  return 1000.0 * timestamps[frameid];
}

double KittiCapture::getCurMsec() const
{
  return getFrameMsec(curframe);
}

int KittiCapture::getCurFrameNum() const
{
  return curframe;
}

bool KittiCapture::setCurFrame(int frameid)
{
  if (frameid < 0 || frameid >= getFramesNum())
    return false;
  curframe = frameid;
  return true;
}
double KittiCapture::getFps() const
{
  return fps;
}