// niblack.cpp

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include "niblack.h"

using namespace std;
using namespace cv;

int niblack( cv::Mat1b& src, cv::Mat1b& res
            , int wing_min // = 5
            , bool invert 
            )
{
  res = src.clone();
  //int wing_min = 5;
  int wing_max = wing_min; // *8;

  Mat sum, sumsq;
  cv::integral( src, sum, sumsq );

  double avesigma = 0;
  int avesigma_cnt = 0;

  Mat e( src.rows, src.cols, CV_8U ); // average at [y x]
  Mat d( src.rows, src.cols, CV_8U ); // sigma at [y x]

  for (int y=0; y<src.rows; y++)
    for (int x=0; x<src.cols; x++)
    {
      for (int wing = wing_min; wing <= wing_max; wing*=2 )
      {
				int sizex = min(wing, x) + min(wing, src.cols - x - 1) + 1;
				int sizey = min(wing, y) + min(wing, src.rows - y - 1) + 1;
				double size = sizex * sizey;

        Point from = Point(max(0, x - wing), max(0, y - wing));
				Point to = Point(min(src.cols, x + wing + 1), min(src.rows, y + wing + 1));
				int s = sum.at<int>(to.y, to.x) - sum.at<int>(from.y, to.x) - sum.at<int>(to.y, from.x) + sum.at<int>(from.y, from.x);
				double sq = sumsq.at<double>(to.y, to.x) - sumsq.at<double>(from.y, to.x) - sumsq.at<double>(to.y, from.x) + sumsq.at<double>(from.y, from.x);
				
				double E = double(s) / size;
        e.at<uchar>(y,x) = saturate_cast<uchar>( E );

				double sigma = sqrt( (double(sq) / size) - E*E );
        d.at< uchar >(y,x) = saturate_cast<uchar>( sigma );

        avesigma += sigma;
        avesigma_cnt ++;

        double koeff = 0.2;

        if ( koeff * sigma < 4 && (wing/wing_min) >= 4 )
        {
          res[y][x] = E > 128 ? 255 : 64; /////(128+64) : 64;
          break;
        }

        int T1 = int( E - koeff * sigma  +0.5 );
        int T2 = int( E + koeff * sigma  +0.5 );


        int srcxy = src[y][x];

        int lo = invert ? 255 : 0;
        int hi = invert ? 0 : 255;

        if ( srcxy < T1 )
        {
          res[y][x] = lo;
          break;
        }
        else if ( srcxy > T2 )
        {
          res[y][x] = hi;
          break;
        }
        else
        {
          res[y][x] = 128;
        }
      }
    }

  avesigma /= avesigma_cnt;
  printf( "avesigma = %f", avesigma );
  
  Mat dbin( src.rows, src.cols, CV_8U ); 
  double thr1 = threshold(d, dbin, 0, 255, THRESH_OTSU);
  printf("Thresh sigma %f", thr1 );

  for (int i=0; i< dbin.rows; i++)
    for (int j=0; j< dbin.cols; j++)
      dbin.at<uchar>(i,j) = 255 - dbin.at<uchar>(i,j);
  //imshow( "blocks", dbin );

  double koeff = 0.15; // 0.2;

  for (int y=0; y<src.rows; y++)
    for (int x=0; x<src.cols; x++)
    {
      int ee = e.at<uchar>(y, x);;
      int dd = d.at<uchar>(y, x);;
      if (dd < thr1)
      {
        res[y][x]=240;
        continue;
      }


      int srcxy = src[y][x];
      int T1 = int( ee - koeff * dd  +0.5 );
      int T2 = int( ee + koeff * dd  +0.5 );
      res[y][x]=(srcxy > T1) ? 255 : 0;

    }

  //imshow( "binarized niblack ex", res );
  //waitKey(0);

  return 0;
}


int niblack_ex( cv::Mat1b& src, cv::Mat1b& res, bool invert )
{
  res = src.clone();
  int wing_min = 7;
  int wing_max = wing_min; // *8;

  Mat sum, sumsq;
  cv::integral( src, sum, sumsq );

  double avesigma = 0;
  int avesigma_cnt = 0;

  Mat e( src.rows, src.cols, CV_8U ); // average at [y x]
  Mat d( src.rows, src.cols, CV_8U ); // sigma at [y x]

  for (int y=0; y<src.rows; y++)
    for (int x=0; x<src.cols; x++)
    {
      for (int wing = wing_min; wing <= wing_max; wing*=2 )
      {
				int sizex = min(wing, x) + min(wing, src.cols - x - 1) + 1;
				int sizey = min(wing, y) + min(wing, src.rows - y - 1) + 1;
				double size = sizex * sizey;

        Point from = Point(max(0, x - wing), max(0, y - wing));
				Point to = Point(min(src.cols, x + wing + 1), min(src.rows, y + wing + 1));
				int s = sum.at<int>(to.y, to.x) - sum.at<int>(from.y, to.x) - sum.at<int>(to.y, from.x) + sum.at<int>(from.y, from.x);
				double sq = sumsq.at<double>(to.y, to.x) - sumsq.at<double>(from.y, to.x) - sumsq.at<double>(to.y, from.x) + sumsq.at<double>(from.y, from.x);
				
				double E = double(s) / size;
        e.at<uchar>(y,x) = saturate_cast<uchar>( E );

				double sigma = sqrt( (double(sq) / size) - E*E );
        d.at< uchar >(y,x) = saturate_cast<uchar>( sigma );

        avesigma += sigma;
        avesigma_cnt ++;

        double koeff = 0.2;

        if ( koeff * sigma < 4 && (wing/wing_min) >= 4 )
        {
          res[y][x] = E > 128 ? 255 : 64; /////(128+64) : 64;
          break;
        }

        int T1 = int( E - koeff * sigma  +0.5 );
        int T2 = int( E + koeff * sigma  +0.5 );


        int srcxy = src[y][x];

        int lo = invert ? 255 : 0;
        int hi = invert ? 0 : 255;

        if ( srcxy < T1 )
        {
          res[y][x] = lo;
          break;
        }
        else if ( srcxy > T2 )
        {
          res[y][x] = hi;
          break;
        }
        else
        {
          res[y][x] = 128;
        }
      }
    }

  avesigma /= avesigma_cnt;
  printf( "avesigma = %f", avesigma );
  
  Mat dbin( src.rows, src.cols, CV_8U ); 
  double thr1 = threshold(d, dbin, 0, 255, THRESH_OTSU);
  printf("Thresh sigma %f", thr1 );

  for (int i=0; i< dbin.rows; i++)
    for (int j=0; j< dbin.cols; j++)
      dbin.at<uchar>(i,j) = 255 - dbin.at<uchar>(i,j);
  imshow( "blocks", dbin );

  double koeff = 0.2;

  for (int y=0; y<src.rows; y++)
    for (int x=0; x<src.cols; x++)
    {
      int ee = e.at<uchar>(y, x);;
      int dd = d.at<uchar>(y, x);;
      if (dd < thr1)
      {
        res[y][x]=240;
        continue;
      }


      int srcxy = src[y][x];
      int T1 = int( ee - koeff * dd  +0.5 );
      int T2 = int( ee + koeff * dd  +0.5 );
      res[y][x]=(srcxy > T1) ? 255 : 0;

    }

  return 0;
}