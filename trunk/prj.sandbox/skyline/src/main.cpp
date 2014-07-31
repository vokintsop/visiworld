#include <conio.h> // _getch()
#include <iostream>
#include <fstream>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


void skyline_single_level( Mat1b& grey, Mat& draw )
{
  Mat1i sum;
  integral( grey, sum );
  double tot = sum[sum.rows-1][sum.cols-1];
  double mots = -1;  int my = -1; // record max
  for (int y=0;y<sum.rows;y++)
  {
    double u = sum[y][sum.cols-1]; int nu = y;
    double d = tot - u; int nd = sum.rows-y;
    double ots_u = nu ? u*u/nu : 0;
    double ots_d = nd ? d*d/nd : 0;
    double ots = ots_u + ots_d;
    
    if (ots > mots)
    {
      mots = ots;
      my = y;
    }
  }
  line( draw, Point(0, my), Point( grey.cols-1, my ), Scalar( 255, 255, 255 ), 3 );
  line( draw, Point(0, my), Point( grey.cols-1, my ), Scalar( 0, 0, 0 ), 1 );
}

void skyline_tiled( Mat1b& grey, Mat& draw )
{
  Mat1i sum;
  integral( grey, sum );
  int xx = sum.cols/10;
  for (int x=0; x<sum.cols-xx; x++)
  {
    double mots = -1;
    int my = -1;
    double tot = sum[0][x] + sum[sum.rows-1][x+xx] - sum[sum.rows-1][x] - sum[0][x+xx];
    for (int y=0;y<sum.rows;y++)
    {
      double u = sum[0][x] + sum[y][x+xx] - sum[y][x] - sum[0][x+xx]; int nu = y;
      double d = tot - u; int nd = sum.rows-y;
      double ots_u = nu ? u*u/nu : 0;
      double ots_d = nd ? d*d/nd : 0;
      double ots = ots_u + ots_d;
      
      if (ots > mots)
      {
        mots = ots;
        my = y;
      }
    }
    line( draw, Point(x, my), Point( x+xx, my ), Scalar( 255, 255, 255 ), 3 );
    line( draw, Point(x, my), Point( x+xx, my ), Scalar( 0, 0, 0 ), 1 );
  }
  imshow( "tiled", draw );

}

bool process_image_file( const string& image_file_name )
{
  cout << image_file_name;
  Mat1b grey = imread( image_file_name, IMREAD_GRAYSCALE );
  Mat1b draw = grey.clone();

  //skyline_single_level( grey );
  skyline_tiled( grey, draw );

  if (27==waitKey(0))
    return false;
  return true;
}

int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string foldername = exe + "/../../../testdata/skyline/";
  string lst = exe + "/../../../testdata/skyline/skyline.lst";

  ifstream inn( lst.c_str() );
  if (!inn.is_open())
    cout << "Can't open " << lst;
  else
  {
    while ( inn.good() )
    {
      string filename;
      inn >> filename;
      if (filename.empty())
        break;
      string pathname = foldername+filename;
      if (!process_image_file( pathname ))
        break;
    }
  }
	return 0;
}