// detect_segment.cpp  
#include "ocvutils/precomp.h"
#include "detect/detect_segment.h"
#include "fht.h"

#define NO_TICKER_MESSAGES
#include "ocvutils/ticker.h"


bool DetectSegment( 
	cv::Mat1i& FHT, 
	std::vector< cv::Point >& pts,
  int k, // коеффициент сжатия
  int rows// размер исходного изображения
)
{
   //imshow ("FHT", FHT);
   for (int i = 0; i < pts.size(); ++i)
     pts[i].x /= k;
   cout << "DetectSegment(" << pts << ")...";
  // cvWaitKey(0);
   if (pts.size() == 1)
   {
     pts.resize(2);
     pair <Point, Point> res;
     res = find_vertical_line_from_one_pt(FHT, pts[0]);
     res = vertical_line_from_segment(rows, res.first, res.second);
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
     res = vertical_line_from_segment(rows, res.first, res.second);

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