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


char* files[]=
{
  //"/../../../testdata/colors_matcher/color-scheme-303-main.png",
  //"/../../../testdata/colors_matcher/color-scheme-400-main.png",
  "/../../../testdata/colors_matcher/apple.png",
  "/../../../testdata/colors_matcher/apple2.png",
  "/../../../testdata/colors_matcher/quelle1.png",
  "/../../../testdata/colors_matcher/quelle2.png",
  "/../../../testdata/colors_matcher/quelle3.png",
  "/../../../testdata/colors_matcher/quelle4.png",
  "/../../../testdata/colors_matcher/quelle5.png",
  "/../../../testdata/colors_matcher/quelle6.png",
  "/../../../testdata/colors_matcher/quelle6-1.png",
  "/../../../testdata/colors_matcher/quelle7.png",
  "/../../../testdata/colors_matcher/quelle8.png",
  "/../../../testdata/colors_matcher/quelle8-1.png",
  "/../../../testdata/colors_matcher/quelle8-2.png",
  "/../../../testdata/colors_matcher/quelle9.png",
  "/../../../testdata/colors_matcher/quelle9-1.png"
};


void   draw_keys( Mat3b& hs, int keys_height, int shift=0 )
{
  int top = hs.rows - keys_height;
  int bot = hs.rows;
  rectangle( hs, Point( 0, top ), Point( hs.cols, bot ), Scalar( 255, 255, 255, 255 ), CV_FILLED );
  int white_width = hs.cols / 14;
  int black_width = hs.cols / 24;
  for (int w=0; w < 14; w++)
  {
    int gap_color = 64;
    rectangle( hs, Rect( w*hs.cols/14, top, white_width, keys_height ), Scalar( gap_color, gap_color, gap_color, gap_color ), 2 );
  }
  bool is_black[12]={ 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
  for (int b=0; b < 24; b++)
  {
    if ( is_black[ (b + shift) % 12 ] )
      rectangle( hs, Rect( b*hs.cols/24, top, black_width, keys_height*9/14 ), Scalar( 0, 0, 0, 0 ), CV_FILLED );
  }
}
/////////////////////////////////////////////////////////////////

Mat3b bgr_initial; // исходное изображение в BGR;
Mat3b hsv_initial; // исходное изображение в HSV, полученное из bgr_initial
int hue_rotation=0; // текущий поворот 
Mat3b hsv_rotated; // повернутое по hue, полученное поворотом hsv_initial на hue_rotation
Mat3b bgr_rotated; // повернутое по hue, полученное из hsv_rotated
string exe_name; // расположение апликации
string image_file_name; // обрабатываемое изображение

/////////////////////////////////////////////////////////////////

int proc_image( Mat3b& bgr, Mat3b& hsv );
bool load_file( string& exe, int i_file )
{
  string data = exe + files[i_file]; ////"/../../../testdata/colors_matcher/quelle8-2.png"; //apple2.png";
  cout << data << endl;

  bgr_initial =  imread( data );
  if (bgr_initial.empty())
    return false;

  imshow( "bgr_initial", bgr_initial );
  cvtColor( bgr_initial, hsv_initial, COLOR_BGR2HSV );

  hue_rotation=0;
  hsv_rotated = hsv_initial.clone();
  cvtColor( hsv_rotated, bgr_rotated, COLOR_HSV2BGR );
  imshow( "bgr_rotated", bgr_rotated );

  return true;

}

int proc_image( Mat3b& bgr, Mat3b& hsv )
{
  int sut_height = 256*2;
  int keys_height = 128;
  Mat3b hs( sut_height+keys_height, 180*2*2, CV_8UC3 );

  
  for (int phi=0; phi < 24; phi++)
  {
    int col = phi%6 ? 100 : 200; 
 
    line( hs, Point( (phi*hs.cols+12)/24, 0 ), Point(  (phi*hs.cols+12)/24, hs.rows ), Scalar(col,col,col,col), phi%12?1:2 );
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
  draw_keys(hs, keys_height);
  imshow("hs", hs);
  imwrite(image_file_name + ".hs.png", hs);

  return waitKey(0);
}

void rotate_hue( int delta )
{
  hue_rotation = (hue_rotation + 180 + delta) % 180;
  cout << "hue_rotation = " << hue_rotation << endl;
  for (int x=0; x<hsv_initial.cols; x++)
    for (int y=0; y<hsv_initial.rows; y++)
    {
      Vec3b pix = hsv_initial[y][x];
      pix[0] = (hue_rotation + pix[0])%180;
      hsv_rotated[y][x] = pix;
    }
  cvtColor( hsv_rotated, bgr_rotated, COLOR_HSV2BGR );
  imshow( "bgr_rotated", bgr_rotated );
}

const int kEscape = 27;
const int kPlus = 43;
const int kEquality = 61; // "=" has same button with "+"
const int kMinus = 45;
const int kLeftArrow =2424832; // отрицательный поворот hue
const int kRightArrow =2555904; // положительный поворот hue

int main( int argc, char* argv[] )
{
  string exe  = argv[0];

  int nfiles = sizeof(files)/sizeof(files[0]);
  
  int i_file = 0;
  if (!load_file( exe, i_file))
  {
    cout << "Can't read image" << endl;
    return waitKey(0);
  }

  int key = -1;
  for (; key != 27; )
  {
    key = proc_image( bgr_rotated, hsv_rotated );
    switch (key)
    {
    case 27: break;
    case kPlus:      i_file = (i_file+2) % nfiles; // +2 -1 = +1
    case kMinus:     i_file = (i_file+nfiles-1) % nfiles; 

      if (!load_file(exe, i_file))
      {
        cout << "Can't read image" << endl;
        return waitKey(0);
      }
      break;

    case kLeftArrow:
      i_file = (i_file+nfiles-1) % nfiles;
      rotate_hue(-1);
      break;
    case kRightArrow:
      rotate_hue(+1);
      break;

    default:
      cout << key << endl;
    }
  }

	return key;
}