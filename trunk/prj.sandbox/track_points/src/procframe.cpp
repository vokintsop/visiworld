
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;



int // 0-continue, <0 failed or user breaks
preprocess_frame( Mat& img )
{
  const double TRUNCATE_TOP = 0.22;
  const double TRUNCATE_HEIGHT = 1 - TRUNCATE_TOP - 0.15;

  int yy = img.rows * TRUNCATE_TOP; // желаемый старт сверху
  int hh = img.rows * TRUNCATE_HEIGHT; // желаемая высота
  Rect roi( 0, yy, img.cols, min( hh, img.rows-yy ) ); ////// <<< danger
  img = img( roi );
  return 0;
}


int // 0-continue, <0 failed or user breaks
procframe( Mat& img, int iframe, double fps )
{

  if (0>preprocess_frame(img))
    return -2;

  imshow("..", img);
  if (27==waitKey(1))
    return -1;

  return 0;
}
