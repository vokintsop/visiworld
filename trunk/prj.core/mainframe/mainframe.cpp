// mainframe.cpp -- главное окно и тп
#include "ocvutils/precomp.h"
#include "mainframe.h"

MarkupMainFrame theFrame;

int MarkupMainFrame::waitKey( int delay )
{
  // select active window
  //...
  // if none -- ask others
  //...

  return cv::waitKey(delay);
}

int WaitKey( int delay )// централизованный обработчик cv::waitKey(), копит клавиши в буфере, распределяет клавиши для обработки между окнами
{
  return theFrame.waitKey(delay); //пока так
}
