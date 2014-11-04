#include "streetglass/coreproc.h"

using namespace cv;

bool CoreProc::process( cv::Mat& frame )
{
  FrameProc fp;
  if (!fp.process( frame, /*FP_BLUCC |*/ FP_REDCC | FP_GRECC ))
    return false;

  int new_state = CP_STATE_UNKNOWN;
  cv::Rect roi( frame.cols*0.2, 0, frame.cols*0.6,  frame.rows*0.5 ); 

  int red_outside=0, red_inside=0;
  for (int i=1; i< int( fp.redcc.cc.size()); i++)
  {
    CCData& cd = fp.redcc.cc[i];
    if (cd.flags && CC_FILTERED)
      continue;

    if (  cd.minx > roi.x && cd.maxx < roi.x+roi.width &&
          cd.miny > roi.y && cd.maxy < roi.y+roi.height
        )
    {
      new_state = CP_STATE_STOP;
      red_inside++;
    }
    else
      red_outside++;
  }

  int gre_outside=0, gre_inside=0;
  for (int i=1; i< int( fp.grecc.cc.size()); i++)
  {
    CCData& cd = fp.grecc.cc[i];
    if (cd.flags && CC_FILTERED)
      continue;

    if (  cd.minx > roi.x && cd.maxx < roi.x+roi.width &&
          cd.miny > roi.y && cd.maxy < roi.y+roi.height
        )
      gre_inside++;
    else
      gre_outside++;
  }

  if (red_inside == 0 && gre_inside == 0)
    last_state.go = 0.9*last_state.go + 0.1*0.5; // тянем к "не знаю", 0.5

  if (red_inside > 0)
  {
    last_state.go   *= 0.7; 
  } else if (red_outside > 0)
  {
    last_state.go   *= 0.9;
  }

  if (gre_inside > 0)
  {
    last_state.go = 1 - (1 - last_state.go )*0.7;
  }


  int DISPLAY_HEIGHT = 360;
  int DISPLAY_WIDTH = 640;

  if (last_state.go > 0.90)
  {
    cv::rectangle( fp.bgr720, roi, Scalar( 0, 255, 0 ), 4 ); // huge green
  } else if (last_state.go > 0.70)
  {
    cv::rectangle( fp.bgr720, roi, Scalar( 0, 255, 0 ), 2 ); // green
  } else if (last_state.go > 0.40)
  {
    cv::rectangle( fp.bgr720, roi, Scalar( 0, 255, 255 ), 2 ); // yellow
  } else if (last_state.go > 0.30)
  {
    cv::rectangle( fp.bgr720, roi, Scalar( 0, 0, 255 ), 2 ); // red
  } else // if (last_state.go <= 0.30)
  {
    cv::rectangle( fp.bgr720, roi, Scalar( 0, 0, 255 ), 4 ); // huge red
  } 

  cv::resize( fp.bgr720, display, cv::Size( DISPLAY_WIDTH, DISPLAY_HEIGHT ) );
  return true;
};



