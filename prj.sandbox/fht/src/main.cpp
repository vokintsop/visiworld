#include <conio.h> // _getch()
#include <iostream>
#include <fstream>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> 

#include "fht.h"

using namespace std;
using namespace cv;

void image_proc (Mat &frame)
{
   Mat1b grayimg;
	 cvtColor(frame, grayimg, CV_RGB2GRAY);
	
   Mat1b gr1;
   resize(grayimg, gr1, Size(grayimg.cols / 3, grayimg.rows));
   grayimg = gr1;

   Mat1b transform = grayimg - grayimg;
   for (int y = 0; y < grayimg.rows - 1; ++y)
   {
     for (int x = 0; x < grayimg.cols - 1; ++x)
     {
        transform(y, x) = abs(grayimg(y, x) - grayimg(y, x + 1));
     }
   }
   blur(transform, transform, Size(3, 3));
   imshow ("input", transform * 10);
   //cvWaitKey(0);
   Mat1i outputl, outputr;
   //fht_vertical(transform, outputl, outputr);
   //test_Haugh(transform);
   test_find_vertical_line(transform);
}

int main(int argc, char * argv[])
{
  if (argc < 2)
    return -1;
  VideoCapture capture(argv[1]);
//  VideoCapture capture("C:/visiroad_3/testdata/left0029.avi");
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