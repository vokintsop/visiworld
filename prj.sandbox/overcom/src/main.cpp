#include <conio.h> // _getch()

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void estimate_overcom( const char* path )
{
  Mat img;
  try { 
    img = imread( path ); }
  catch (...) {
    return; }
  if (img.empty())
    return;

  cout << path << endl;

  int hor[8][3]={0};
  int ver[8][3]={0};
  for (int y=0; y<img.rows-1; y++)
  {
    for (int x=0; x<img.cols-1; x++)
    {
      Vec3b pix00 = img.at<Vec3b>(y,x);
      Vec3b pix01 = img.at<Vec3b>(y,x+1);
      Vec3b pix10 = img.at<Vec3b>(y+1,x);
      hor[x%8][0] += abs( pix00[0] - pix01[0] );
      hor[x%8][1] += abs( pix00[1] - pix01[1] );
      hor[x%8][2] += abs( pix00[2] - pix01[2] );
      ver[y%8][0] += abs( pix00[0] - pix10[0] );
      ver[y%8][1] += abs( pix00[1] - pix10[1] );
      ver[y%8][2] += abs( pix00[2] - pix10[2] );
    }
  }
  cout << "--- hor ---\n";
  for ( int c=0; c<3; c++ )
  {
    for (  int i=0; i<8; i++ )
    {
      cout << hor[i][c] << '\t';
    }
    cout << endl;
  }
  cout << "--- ver ---\n";
  for ( int c=0; c<3; c++ )
  {
    for (  int i=0; i<8; i++ )
    {
      cout << ver[i][c] << '\t';
    }
    cout << endl;
  }


}


int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string data = exe + "/../../../testdata/overcom/overcom01/IMG_3463_%02d.jpg";

  for (int i=1; i<100; i++)
    estimate_overcom( format( data.c_str(), i ).c_str() );

  _getch();

	return 0;
}