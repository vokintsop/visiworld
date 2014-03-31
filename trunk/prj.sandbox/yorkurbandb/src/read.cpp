// read.cpp
#include <fstream>

#include "imagerecord.h"

using namespace std;
using namespace cv;

static void AddValues(vector<int> &out, const vector<int> &idxs, const int val) {
  for (size_t i = 0; i < idxs.size(); ++i) {
    out[idxs[i]] = val;
  }
}

bool read_image_records( std::string& root, std::vector< ImageRecord >& image_records )
{
  // todo: fill image_records[] inputs

  string fnames_file = root + "/Manhattan_Image_DB_Names.txt";
  string train_test_idxs_file = root + "/ECCV_TrainingAndTestImageNumbers.txt";

  vector<string> dirs;
  {
    ifstream ifile;
    ifile.open(fnames_file.c_str());
    if (!ifile.good())
      return false;
    while (ifile) {
      string cur_dir;
      ifile >> cur_dir;
      if (cur_dir.length() < 1)
        continue;
      dirs.push_back(cur_dir);
    }
    ifile.close();
  }

  vector<int> test;
  vector<int> train;
  vector<int> *p = NULL;
  int max_idx = 0;

  {
    ifstream ifile;
    ifile.open(train_test_idxs_file.c_str());
    if (!ifile.good())
      return false;
    while (ifile) {
      string cur_num;
      ifile >> cur_num;
      if (cur_num == "trainingSetIndex:") {
        p = &train;
      } else if (cur_num == "testSetIndex:") {
        p = &test;
      } else if (p) {
        int cur_idx = atoi(cur_num.c_str()) - 1; 
        if (cur_idx >= 0) {
          p->push_back(cur_idx);
          max_idx = max(max_idx, cur_idx);
        }
      } else {
        return false;
      }
    }

    ifile.close();
    if (max_idx != dirs.size() - 1) { 
      return false;
    }
  }


  vector<int> flags(dirs.size());
  AddValues(flags, test, 0);
  AddValues(flags, train, 1);

  for (size_t i = 0; i < dirs.size(); ++i) {
    std::string __name = root + "/" + dirs[i] + "/" + dirs[i];
    std::string vplname = __name + "LinesAndVP.txt";
    std::ifstream ifile(vplname.c_str());
    if (!ifile.good())
      return false;
    ImageRecord cur;
    cur.how_to_use = flags[i];
    while (ifile) {
      int x1 = 0, y1 = 0, x2 = 0, y2 = 0, vp = 0;
      ifile >> x1 >> y1 >> x2 >> y2 >> vp;
      cur.name = __name;
      cur.segments.push_back(make_pair(cv::Point(x1, y2), cv::Point(x2, y2)));
    }
    image_records.push_back(cur);
  }

    return true;
}

