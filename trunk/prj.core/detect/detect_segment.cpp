// detect_segment.cpp  
#include "ocvutils/precomp.h"
#include "detect/detect_segment.h"


#define NO_TICKER_MESSAGES
#include "ocvutils/ticker.h"


bool DetectSegment( 
	cv::Mat3b& bgr, 
	std::vector< cv::Point >& pts 
)
{
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