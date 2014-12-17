#include <conio.h> // _getch()
#include <iostream>
#include <fstream>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "fht.h"

using namespace std;
using namespace cv;

void image_proc (Mat &frame)
{
   Mat1b grayimg;
	 cvtColor(frame, grayimg, CV_RGB2GRAY);
	
   Mat1b transform = grayimg - grayimg;
   for (int y = 0; y < grayimg.rows - 1; ++y)
   {
     for (int x = 0; x < grayimg.cols - 1; ++x)
     {
        transform(y, x) = abs(grayimg(y, x) - grayimg(y, x + 1));
     }
   }
   //blur(transform, transform, Size(3, 3));
   imshow ("input", transform * 10);
   //cvWaitKey(0);
   Mat1i outputl, outputr;
   fht_vertical(transform, outputl, outputr);
}

int main()
{
  VideoCapture capture("C:/visiroad_3/testdata/left0029.avi");
  if(!capture.isOpened()){
      std::cout<<"cannot read video!\n";
      return -1;
  }
  Mat frame;
  namedWindow("frame");

  double rate = capture.get(CV_CAP_PROP_FPS);
  int delay = 1000/rate;

  while(true)
  {
      if(!capture.read(frame)){
          break;
      }
      imshow("frame",frame);
      image_proc(frame);
      cvWaitKey(0);

  }

  capture.release();
	return 0;
}