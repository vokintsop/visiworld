#include "kitti.h"
#include "ocvutils/precomp.h"
#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;


bool readTimeStamps(const string &fname, vector<double> &timestamps)
{
  ifstream fin(fname.c_str());
  if (!fin.good())
    return __false(format("\nCan't open timestamp file %s\n", fname));
    
  vector<string> timestamps_str;
  while (fin.good())
  {
    string str;
    getline(fin, str);
    if (!str.empty())
      timestamps_str.push_back(str);
  }
  fin.close();

  for (int i = 0; i < timestamps_str.size(); ++i)
  {
    string p = timestamps_str[i];
    tm time = {0};
    double secs = 0.0;
    if (sscanf_s(p.c_str(), "%d-%d-%d %d:%d:%lf", &time.tm_year, &time.tm_mon, &time.tm_mday,
      &time.tm_hour, &time.tm_min, &secs) == EOF)
      return __false(format("\nWrong forman of timpestamp file: %s", fname));
    double int_secs = 0.0, dec_secs = 0.0;
    dec_secs = modf(secs, &int_secs);
    time.tm_sec = int_secs;
    time.tm_year -= 1900; //так работает функция
    time_t dtime = mktime(&time);
    if (dtime == -1)
      return __false("\nError reading timestamp\n");
    timestamps.push_back(double(mktime(&time)) + dec_secs);
  }
  return true;
}