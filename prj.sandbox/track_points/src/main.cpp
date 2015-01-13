#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

/// Global variables
Mat src, src_gray;
int thresh = 200;
int max_thresh = 255;

char* source_window = "Source image";
char* corners_window = "Corners detected";

/// Function header
void cornerHarris_demo( int, void* );

/** @function main */
int main( int argc, char** argv )
{
  /// Load source image and convert it to gray
  src = imread( "/testdata/visiroad_10.jpg", 1 );
  cvtColor( src, src_gray, cv::COLOR_BGR2GRAY );

  /// Create a window and a trackbar
  namedWindow( source_window, cv::WINDOW_AUTOSIZE );
  createTrackbar( "Threshold: ", source_window, &thresh, max_thresh, cornerHarris_demo );
  imshow( source_window, src );

  cornerHarris_demo( 0, 0 );

  waitKey(0);
  return(0);
}

/** @function cornerHarris_demo */
void cornerHarris_demo( int, void* )
{

  Mat dst, dst_norm, dst_norm_scaled;
  dst = Mat::zeros( src.size(), CV_32FC1 );

  /// Detector parameters
  int blockSize = 2;
  int apertureSize = 3;
  double k = 0.04;

  /// Detecting corners
  cornerHarris( src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT );

  /// Normalizing
  normalize( dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat() );
  convertScaleAbs( dst_norm, dst_norm_scaled );

  /// Drawing a circle around corners
  for( int j = 0; j < dst_norm.rows ; j++ )
     { for( int i = 0; i < dst_norm.cols; i++ )
          {
            if( (int) dst_norm.at<float>(j,i) > thresh )
              {
               circle( dst_norm_scaled, Point( i, j ), 5,  Scalar(0), 2, 8, 0 );
              }
          }
     }
  /// Showing the result
  namedWindow( corners_window, cv::WINDOW_AUTOSIZE );
  imshow( corners_window, dst_norm_scaled );
}


#if 0 /////////////////////
#include <conio.h> // _getch()

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


int // 0-continue, <0 failed or user breaks
procframe( Mat& img, int iframe, double fps );

int // 0-continue, <0 failed or user breaks
procframe( const char* path, int iframe, double fps=0 )
{
  Mat img;
  try { 
    img = imread( path ); }
  catch (...) {
    return -1; }
  if (img.empty())
    return -1;

  cout << path << endl;

  return procframe( img, iframe, fps );
}



int main( int argc, char* argv[] )
{
  //string exe  = argv[0];
  //string data = exe + "/../../../testdata/track_points/overcom01/IMG_3463_%02d.jpg";

  string data = "/testdata/akenori/input/REC.0716/AKN00014.avi"; // EVENING
  //string data = "/testdata/akenori/input/REC.0716/AKN00010.avi"; // DAYLIGHT
  //string data = "/testdata/akenori/input/REC.0716/AKN00008.avi"; // DGFGDFGD

  cv::VideoCapture cap( data );
  double fps = cap.get( CV_CAP_PROP_FPS );
  Mat frame;
  int index=0;
  while (cap.read( frame ))
  {
    //imshow("..", frame);
    //if (27==waitKey(1))
    //  break;
    if (0>procframe( frame, index++, fps ))
      break;
  }

  return 0;


  for (int i=1; i<100; i++)
    procframe( format( data.c_str(), i ).c_str(), i );

  system("pause");

  return 0;
}

#endif