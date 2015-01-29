// detect_segment.cpp  
#include "ocvutils/precomp.h"
#include "detect/detect_segment.h"
#include "fht.h"

#define NO_TICKER_MESSAGES
#include "ocvutils/ticker.h"


bool DetectSegment( 
	cv::Mat3b& bgr, 
	std::vector< cv::Point >& pts 
)
{

  const int k = 5; // коеффициент сжатия

   Mat1b grayimg;
	 cvtColor(bgr, grayimg, CV_RGB2GRAY);

   cout << pts.size() << endl;
	
   Mat1b gr1;
   resize(grayimg, gr1, Size(grayimg.cols / k, grayimg.rows));
   grayimg = gr1;

   Mat1b transform = grayimg - grayimg;
   for (int y = 0; y < grayimg.rows - 1; ++y)
   {
     for (int x = 0; x < grayimg.cols - 1; ++x)
     {
        transform(y, x) = abs(grayimg(y, x) - grayimg(y, x + 1));
     }
   }
   blur(transform, transform, Size(3, 3));
   //imshow ("fht", transform);
  
   Mat1i L, R;
   fht_vertical(transform, L, R);
   Mat1i FHT;
   vertical_sum_fht_l_r(L, R, FHT);
   //imwrite("C:/visiroad_3/FHT.jpg", FHT / 10);

   for (int i = 0; i < pts.size(); ++i)
     pts[i].x /= k;
   cout << "DetectSegment(" << pts << ")...";
  // cvWaitKey(0);
   if (pts.size() == 1)
   {
     pts.resize(2);
     pair <Point, Point> res;
     res = find_vertical_line_from_one_pt(FHT, pts[0]);
     pts.resize(2);
     res.first.x *= k;
     res.second.x *= k;
     pts[0] = res.first;
     pts[1] = res.second;
     --pts[1].y;
     return true;
   }
   if (pts.size() >= 2)// == 2
   {
     pts.resize(2);
     pair <Point, Point> res;
     
     res = find_vertical_line_from_two_pt(FHT, pts[0], pts[1]);
     res.first.x *= k;
     res.second.x *= k;
     pts[0] = res.first;
     pts[1] = res.second;

    // cout << "LLLL" << endl;
     --pts[1].y;
    // cvWaitKey(0);
     cout << pts << endl;
     return true;
   }

   return false;
  
  
  bool ok=true;
  cout << "DetectSegment(" << pts << ")...";
  if (!ok)
  {
    cout << "failed" << endl;
    return false;
  }

  cout << "ok" << endl;
  return false;
}