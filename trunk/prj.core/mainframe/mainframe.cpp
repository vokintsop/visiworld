// mainframe.cpp -- главное окно и тп
#include "ocvutils/precomp.h"
#include "mainframe.h"

int MarkupMainFrame::waitKey( int delay )
{
  // select active window
  //...
  // if none -- ask others
  //...

  return cv::waitKey(delay);
}

