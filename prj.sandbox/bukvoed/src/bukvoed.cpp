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


class CCData
{
public:
  int minx, miny, maxx, maxy; // коробка
  double xc, yc; // центр масс
  double sum_f;
  double sum_fx,sum_fy;
  double sum_fxx, sum_fxy, sum_fyy;
  CCData():
    minx( INT_MAX ), miny( INT_MAX ), maxx( INT_MIN ), maxy(INT_MIN ),
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

Mat1b src; // входное изображение, ч-б, 0-черное (сигнал), 255-белое (фон)
Mat1b src_dilated; // размазанное входное
Mat1i labels; // карта компонент связности, [2...labels.size()-1] ==> cc[]
vector< CCData > cc; // информация о компонентах

void dilate1( Mat& in, Mat& ou, bool inverted = false )
{
  int an = 1;
  int element_shape = MORPH_CROSS; // MORPH_RECT;
  Mat element = getStructuringElement(element_shape, Size(an*2+1, an*2+1), Point(an, an) );
  if (!inverted)
    dilate( in, ou, element, Point( 1, 1 ) );
  else
    erode( in, ou, element, Point( 1, 1 ) );

  imshow( "before_dilation", in );
  imshow( "after_dilation", ou );
  waitKey(0);

}


inline bool more_128( int x, int y, Mat1b& m )
{
  return x < m.cols && x>=0 && y < m.rows && y >= 0 && m[y][x] > 128;
}
class MetrСС // A intersect dilated(B) + B intersect dilated(A)
{
public:
  long long counter; // 
  MetrСС(): counter(0){}

  double computeDistance( const int& i1,  const int& i2 )  // индексы к сс[]
  {
    double dst=0;
    CCData& ccd1 = cc[i1];
    CCData& ccd2 = cc[i2];
    int dx = cvRound( ccd2.xc - ccd1.xc );
    int dy = cvRound( ccd2.yc - ccd1.yc );
    for ( int y=ccd1.miny; y<ccd1.maxy; y++ )
    {
      for ( int x=ccd1.minx; x<ccd1.maxx; x++ )
      {
        if (labels[y][x] == i1) // сигнал
        {
          int qqq = src[y][x];
          assert( src[y][x] < 128 );
          if (more_128( x+dx, y+dy, src_dilated ))
            dst++;
        }
      }
    }

    for ( int y=ccd2.miny; y<ccd2.maxy; y++ )
    {
      for ( int x=ccd2.minx; x<ccd2.maxx; x++ )
      {
        if (labels[y][x] == i2) // сигнал
        {
          assert( src[y][x] < 128 );
          if (more_128( x-dx, y-dy, src_dilated ))
            dst++;
        }
      }
    }

    counter++;
    return dst;
  }
};

int bukvoed( int argc, char* argv[] )
{
	int res = 0;
  //Mat1b 
  src = imread( "/images/1.png", IMREAD_GRAYSCALE );
  //src = imread( "/images/3.png", IMREAD_GRAYSCALE );
  dilate1( src, src_dilated, true );

#if 0
  imshow( "input", src );
  waitKey(0);
#endif

  //src = src < 128;
  Mat1b thr; // 0-background, 1-foreground
  threshold( src, thr, 128., 1., THRESH_BINARY_INV );


  int label_index=0;
  labels = make_labels( thr, label_index );

  cc.clear(); 
  cc.resize( label_index );
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
  for (int i=3; i< int(cc.size()); i++ ) // <<<<<<<<<<< 2 comp -> gpf
  {
    CCData& ccd = cc[i];
    ccd.fix();
#if 0 // draw rects
    rectangle( src, Point( ccd.minx, ccd.miny ), Point( ccd.maxx, ccd.maxy ), Scalar( 128, 0, 0 ) );
#endif
  }
  //imshow( "comps", src );
  //waitKey(0);

  //cout << labels;

  {
    Ticker t;
    MetrСС ruler;
    CoverNet< int, MetrСС > cvnet( &ruler, 10000, 1 );
    for (int i=3; i<int(cc.size()); i++) // <<<<<<<<<<< 2 comp -> gpf
    {
      cvnet.insert(i);
    }
    double ms = t.msecs();
    cout << "\MetrCC metrics (simple L1):" << endl;
    cvnet.reportStatistics( 0, 3 ); 
    cout << "Build time = " << ms/1000 << " seconds" << endl;
  }

  return res;
}

////////////////////////////////////////////////////

/*

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
*/