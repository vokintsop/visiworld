#include "ocvutils/precomp.h"
#include "streetglass/sticks.h"
#include <opencv2/imgproc/imgproc.hpp> 

bool computeVerticalSticsAsMat1b( cv::Mat& img, cv::Mat1b& vsticks )
{
  //Mat a(5, 5,  fff );
  Rect r00( 0,0, img.cols-1, img.rows-1 );
  Rect r10( 1,0, img.cols-1, img.rows-1 );
  Rect r01( 0,1, img.cols-1, img.rows-1 );
  Mat m00 = img(r00);
  Mat m10 = img(r10);
  Mat m01 = img(r01);
  Mat dmh1 = m10-m00, dmh2 = m00-m10; // производная по горизонтали, подчеркивает вертикальные границы
  Mat dmh = dmh1 + dmh2;

  Mat dmv1 = m01-m00, dmv2 = m00-m01; // производная по вертикали, подчеркивает горизонтальные границу
  Mat dmv = dmv1 + dmv2;



  Mat idmh, idmv;
  Mat1f res(img.rows-1, img.cols-1);
  cv::integral( dmh, idmh );
  cv::integral( dmv, idmv );
  int dw = 2;
  int dh = 32; 
  double max_diff=0;
  for ( int x=dw; x < dmh.cols - dw; x++)
  {
    //double prev=0;
    for ( int y=dh; y < dmh.rows - dh; y++)
    {
      Vec3i sh =  idmh.at<Vec3i>(y-dh, x-dw) 
                - idmh.at<Vec3i>(y+dh, x-dw) 
                - idmh.at<Vec3i>(y-dh, x+dw) 
                + idmh.at<Vec3i>(y+dh, x+dw);
      Vec3i sv =  idmv.at<Vec3i>(y-dh, x-dw) 
                - idmv.at<Vec3i>(y+dh, x-dw) 
                - idmv.at<Vec3i>(y-dh, x+dw) 
                + idmv.at<Vec3i>(y+dh, x+dw);

      double diff = norm( sh-sv );
      res[y][x] = diff;
      max_diff = max( diff, max_diff );

      //if (prev <= diff)
      //{
      //  res[y][x] = diff;
      //  res[y][x-1] = 0;
      //  prev = diff;
      //  max_diff = max( diff, max_diff );
      //}
    }
  }
  //cv::Scalar meanres = mean( res );
  //double ratio = 128/(1.+meanres[0]);
  double ratio = 255/(1.+max_diff);
  res *= ratio;
  //imshow( "dmh", dmh );
  //imshow( "dmv", dmv );
  //Mat1b bres( res );
  //imshow("diff", bres);
  vsticks = res;

  //waitKey(0);

  return true;
}