#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <conio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>

using namespace std;
using namespace cv;


  //string data = exe + "/../../../testdata/colors_matcher/color-scheme-303-main.png";
  //string data = exe + "/../../../testdata/colors_matcher/color-scheme-400-main.png";
  //string data = exe + "/../../../testdata/colors_matcher/apple.png";
  //string data = exe + "/../../../testdata/colors_matcher/quelle8-2.png"; //apple2.png";
  //string data = exe + "/../../../testdata/colors_matcher/quelle8-2.png"; //apple2.png";



char* files[]=
{
  "/../../../testdata/colors_matcher/quelle4.png",
  "/../../../testdata/colors_matcher/quelle5.png",
  "/../../../testdata/colors_matcher/quelle6.png",
  "/../../../testdata/colors_matcher/quelle7.png",
  "/../../../testdata/colors_matcher/quelle8.png",
  "/../../../testdata/colors_matcher/quelle8-1.png",
  "/../../../testdata/colors_matcher/quelle8-2.png",
  "/../../../testdata/colors_matcher/quelle9.png",
  "/../../../testdata/colors_matcher/quelle9-1.png"
};


int proc_file( string& exe, int i_file )
{
  string data = exe + files[i_file]; ////"/../../../testdata/colors_matcher/quelle8-2.png"; //apple2.png";
  cout << data << endl;

  Mat3b bgr =  imread( data );
  if (bgr.empty())
  {
    cout << "Can't read image" << endl;
    return waitKey(0);
  }

  imshow( "bgr", bgr );

  Mat3b hsv;
  cvtColor( bgr, hsv, COLOR_BGR2HSV );
  //imshow( "hsv", hsv );

  Mat3b hs( 256*2, 180*2*2, CV_8UC3 );

  
  for (int phi=0; phi < 24; phi++)
  {
    int col = phi%6 ? 100 : 200; 
 
    line( hs, Point( phi*hs.cols/24, 0 ), Point(  phi*hs.cols/24, hs.rows ), Scalar(col,col,col,col), phi%12?1:2 );
  }

  for (int x=0; x<hsv.cols; x++)
    for (int y=0; y<hsv.rows; y++)
    {
      Vec3b pix = hsv[y][x];
      int h = pix[0];
      int s = pix[1];
#if 1
      hs[2*s+0][2*h+0] = bgr[y][x];
      hs[2*s+1][2*h+0] = bgr[y][x];
      hs[2*s+0][2*h+1] = bgr[y][x];
      hs[2*s+1][2*h+1] = bgr[y][x];
      
      hs[2*s+0][2*h+360] = bgr[y][x];
      hs[2*s+1][2*h+360] = bgr[y][x];
      hs[2*s+0][2*h+361] = bgr[y][x];
      hs[2*s+1][2*h+361] = bgr[y][x];


#else
      circle( hs, Point( pix[0], pix[1]), 3, Scalar( bgr[y][x][0], bgr[y][x][1], bgr[y][x][2] ), 3 ); 
#endif
    }
  imshow("hs", hs);
  imwrite(data+".hs.png", hs);

  return waitKey(0);
}

const int kEscape = 27;
const int kPlus = 43;
const int kEquality = 61; // "=" has same button with "+"
const int kMinus = 45;

int main( int argc, char* argv[] )
{
  string exe  = argv[0];

  int key=-1;
  int nfiles = sizeof(files)/sizeof(files[0]);
  for ( int i_file = 0; key != 27; )
  {
    switch (key=proc_file( exe, i_file))
    {
    case 27: break;
    case kMinus:
      i_file = (i_file+nfiles-1) % nfiles; break;
    case kPlus:
    default:
      i_file = (i_file+1) % nfiles;
    }
  }

	return 0;
}