// bukvoed
#include <conio.h>
#include <cassert>
#include <climits>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "cover_net.h"
#include "ticker.h"

using namespace std;
using namespace cv;


Mat make_labels( const cv::Mat &bw, int& label_index )
{
  Ticker t;  

  Mat labels;
  bw.convertTo(labels, CV_32SC1);
  label_index = 2; 
  for(int y=0; y < labels.rows; y++)
  {
    int *row = (int*)labels.ptr(y);
    for(int x=0; x < labels.cols; x++) 
    {
      if( row[x] != 1 )
          continue;
      cv::Rect rect;
      cv::floodFill(labels, cv::Point(x,y), label_index, &rect, 0, 0, 4);
      label_index++;
    }
  }
  cout << "make_labels() ... " << t.msecs() << " milliseconds" << endl;
  return labels;
}



void comps( Mat1b& src )
{
  src = src < 128;

  vector< vector<Point> > contours;
  vector< Vec4i > hierarchy;
  { 
    Ticker t;  
    findContours( src, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    cout << "findContours()..." << t.msecs() << " milliseconds" << endl;
  }


  // iterate through all the top-level contours,
  // draw each connected component with its own random color

  Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);

  int idx = 0;
  for( ; idx >= 0; idx = hierarchy[idx][0] )
  {
      Scalar color( rand()&255, rand()&255, rand()&255 );
      drawContours( dst, contours, idx, color, CV_FILLED, 8, hierarchy );
  }

  namedWindow( "Components", 1 );
  imshow( "Components", dst );
  waitKey(0);
}

class CCData
{
public:
  int minx, miny, maxx, maxy; // коробка
  double xc, yc; // центр масс
  double sum_f;
  double sum_fx,sum_fy;
  double sum_fxx, sum_fxy, sum_fyy;
  CCData():
    minx( INT_MIN ), miny( INT_MIN ), maxx( INT_MAX ), maxy(INT_MAX),
    sum_f(0.), sum_fx(0.), sum_fy(0.), sum_fxx(0.), sum_fxy(0.), sum_fyy(0.)
  {}
  void add( int x, int y, double f = 1. )
  {
    minx = min( x, minx );     miny = min( y, miny ); 
    maxx = max( x, maxx );     maxy = max( y, maxy ); 
    sum_f += f;
    sum_fx += f*x; 
    sum_fy += f*y;
    sum_fxx += f * x * x;  
    sum_fxy += f * x * y;
    sum_fyy += f * y * y;
  }
  void fix()
  {
    xc = sum_fx / sum_f;
    yc = sum_fy / sum_f;
    /*
    // sum_fxx_ == sum( f * (x-xc) * (x-xc) ) == sum( f*x*x - 2*f*x*xc + f*xc*xc ) ==
    // == sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc
    double sum_fxx_ = sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc;
    double sum_fyy_ = sum_fyy - 2*sum_fy * yc  + sum_f * yc * yc;

    // fxy_ == sum( f * (x-xc) * (y-yc) ) == sum( f*x*x - f*x*yc - f*y*xc + f*xc*yc ) ==
    // == sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc
    double sum_fxy_ = sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc;

    double ds = sum_fxx_ - sum_fyy_;
    double phi_rad =  ( abs( ds ) <= 0.000001 ) ? 0. 
      : 0.5 * atan( 2*sum_fxy_ / ds );
      //: 0.5 * atan2( 2*sum_fxy_ , ds );

    double I1 = 0.5*( sum_fxx_ + sum_fyy_  + sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));
    double I2 = 0.5*( sum_fxx_ + sum_fyy_  - sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));

    double phi_degree = phi_rad * 360./ (CV_PI*2);
    */
  }
};

int bukvoed( int argc, char* argv[] )
{
	int res = 0;
  //Mat1b src = imread( "/images/1.png", IMREAD_GRAYSCALE );
  Mat1b src = imread( "/images/3.png", IMREAD_GRAYSCALE );
#if 0
  imshow( "input", src );
  waitKey(0);
#endif

  int label_index=0;
  //src = src < 128;
  threshold( src, src, 128., 1., THRESH_BINARY_INV );

#if 0
  imshow( "input-0-1", src );
  waitKey(0);
#endif

  Mat1i labels = make_labels( src, label_index );

  vector< CCData > cc( label_index );
  for(int y=0; y < labels.rows; y++)
  {
    int *row = (int*)labels.ptr(y);
    for(int x=0; x < labels.cols; x++) 
    {
      if( row[x] <= 1 )
          continue;
      cc[ row[x] ].add( x, y );
    }
  }
  for (int i=2; i< int(cc.size()); i++ )
    cc[i].fix();


  cout << labels;


  return res;
}