// explore mnist dataset
// http://yann.lecun.com/exdb/mnist/

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>


#include "cover_tree.h"
//#include <Winsock2.h>

using namespace std;
using namespace cv;

vector< pair< char, cv::Mat > > tst_samples;
vector< pair< char, cv::Mat > > trn_samples;

inline unsigned long ntohl(unsigned long n) {
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

void read_samples(string &folder, vector< pair< char, cv::Mat > > &samples)
{
  ifstream cin ((folder + "list.txt").c_str());
  string filename;

  int xsize = 0, ysize = 0;
   
  while (cin >> filename)
  {
    char letter = filename[0];
    if (filename[0] == 'S' && filename[1] == 'P')
    {
      letter = ' ';
    }
    if (filename[0] == 'S' && filename[1] == 'L')
    {
      letter = '/';
    }
    if (filename[0] == 'C' && filename[1] == 'O')
    {
      letter = ':';
    }

    //cout << filename << endl;
    Mat img = imread((folder + filename).c_str());
    Mat gray;
    cvtColor(img, gray, CV_RGB2GRAY);
    xsize = max(img.cols, xsize);
    ysize = max(img.rows, ysize);

    samples.push_back(make_pair(letter, gray));
 }

 // cout << xsize << " " << ysize << endl;
 
  cout << samples.size() << " samples read from " << folder << endl;
}

bool write_samples( string &imgfile, string &labelfile, vector<pair<char, Mat> > &samples )
{	
	ofstream out(imgfile.c_str(), ios::binary|ios::out);
  ofstream out1(labelfile.c_str(), ios::binary|ios::out);
 	
	int magic_number = 2051;
  magic_number = ntohl(magic_number);
  out.write(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
	out1.write(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
	
  int img_num = samples.size();
  img_num = ntohl(img_num);
  out.write(reinterpret_cast<char*>(&img_num), sizeof(img_num));
  out1.write(reinterpret_cast<char*>(&img_num), sizeof(img_num));
	int rows = 32;
  rows = ntohl(rows);
	out.write(reinterpret_cast<char*>(&rows), sizeof(rows));
  
  int cols = 21;
  cols = ntohl(cols);


	
  //SAMPLE_HEIGHT = rows;
	
	
	out.write(reinterpret_cast<char*>(&cols), sizeof(cols));
  //SAMPLE_WIDTH = cols;

	for (int i = 0; i < (int)samples.size(); ++i)
	{
    Mat1b mymat = samples[i].second;
    resize(samples[i].second, mymat, Size(21, 32));
    //imshow("res", mymat);
    cvWaitKey(0);
    unsigned char label = samples[i].first;
   // cout << i << " " << label << endl;
		out1.write(reinterpret_cast<char*>(&label), sizeof(label));
		for (int i1 = 0; i1 < 32; ++i1)
		{
			for (int i2 = 0; i2 < 21; ++i2)
			{
				unsigned char pixel = 0;
        if (i1 < mymat.rows && i2 < mymat.cols)
          pixel = mymat(i1, i2);
				out.write(reinterpret_cast<char*>(&pixel), sizeof(pixel));
			}
		}
	}

  cout << samples.size() << " samples write"<< endl;;

  return true;
}


int main( int argc, char* argv[] )
{
  //string exe = argv[0];
  string beg = "/testdata/idcards/characters/";
  string tst_folder = "/testdata/idcards/characters/test_set/";
	read_samples(tst_folder, tst_samples);

  string trn_folder = "C:/visiroad_3/idcards/characters/train_set/";
  read_samples(trn_folder, trn_samples);
  

  write_samples(beg + "/train-images.idx3-ubyte", beg + "/train-labels.idx1-ubyte", trn_samples);
  write_samples(beg + "/t10k-images.idx3-ubyte", beg + "/t10k-labels.idx1-ubyte", tst_samples);

	return 0;
}