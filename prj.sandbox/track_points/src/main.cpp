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

  string data = "/testdata/akenori/input/REC.0716/AKN00014.avi";

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