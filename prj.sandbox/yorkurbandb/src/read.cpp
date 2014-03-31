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

  const string fnames_file = root + "/Manhattan_Image_DB_Names.txt";
  const string train_test_idxs_file = root + "/ECCV_TrainingAndTestImageNumbers.txt";
  const string cam_param_file = root + "/cameraParameters.txt";

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

  double flen_mm = 0.0;
  double pixsize = 0.0;

  {
    ifstream ifile(cam_param_file.c_str());
    string flen, psize, tmp0, tmp1;
    ifile >> tmp0 >> flen >> tmp1 >> psize;
    flen_mm = atof(flen.c_str());
    pixsize = atof(psize.c_str());
    ifile.close();
  }

  int width  = 640; // todo - read,initialize in pixels
  int height = 480; // todo - read,initialize in pixels
  int depth  = (flen_mm / pixsize) + 0.5; // ????????? todo - read,initialize in pixels
  HCoords hcoords( width, height, depth ); // все в пикселях


  vector<int> flags(dirs.size());
  AddValues(flags, test, 0);
  AddValues(flags, train, 1);

  for (size_t i = 0; i < dirs.size(); ++i) {
    ImageRecord cur;
    string __name = root + "/" + dirs[i] + "/" + dirs[i];
    string vplname = __name + "LinesAndVP.txt";
    {
      ifstream ifile(vplname.c_str());
      if (!ifile.good())
        return false;
      cur.how_to_use = flags[i];
      cur.hcoords = hcoords;
      while (ifile) {
        int x1 = 0, y1 = 0, x2 = 0, y2 = 0, vp = 0;
        ifile >> x1 >> y1 >> x2 >> y2 >> vp;
        cur.name = __name;
        cur.segments.push_back(make_pair(cv::Point(x1, y2), cv::Point(x2, y2)));
      }
      ifile.close();
    }

    string gtvp = __name + "GroundTruthVP_CamParams.txt";
    {
      ifstream ifile(gtvp.c_str());
      if (!ifile.good())
        return false;
      for (int i = 0; i < 3; ++i) {
        ifile >> cur.truth[i].x >> cur.truth[i].y >> cur.truth[i].z;
      }
      ifile >> cur.sigma[0] >> cur.sigma[1] >> cur.sigma[2];
      ifile.close();
    }

    string gtvp_ort = __name + "GroundTruthVP_Orthogonal_CamParams.txt";
    {
      ifstream ifile(gtvp_ort.c_str());
      if (!ifile.good())
        return false;
      for (int i = 0; i < 3; ++i) {
        ifile >> cur.truth_ort[i].x >> cur.truth_ort[i].y >> cur.truth_ort[i].z;
      }
      ifile.close();
    }

    image_records.push_back(cur);
  }

    return true;
}

