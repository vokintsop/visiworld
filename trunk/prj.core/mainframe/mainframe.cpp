// mainframe.cpp -- главное окно и тп
#include "ocvutils/precomp.h"
#include "mainframe.h"

MarkupMainFrame theFrame;

int MarkupMainFrame::waitKey( int delay )
{
  int key = cv::waitKey(delay);

  if (key == kNoKeyPressed)
    return key;

  HWND hWnd = GetActiveWindow();

  char lpBuff[MAX_PATH] = {0};
  int ret = GetWindowText(hWnd, lpBuff, MAX_PATH);

  std::string strWindowTitle(lpBuff);
  std::string strAGM = "type=AGM_";
  int nf = strWindowTitle.find(strAGM);
  if (std::string::npos != nf)
  {
    this->pGeoMapEditor->pressed_keys.push_back(key);
    this->pGeoMapEditor->processKeys();
    if (key == kTab || key == kBackSpace)
    {
      key = kNoKeyPressed;
    }
  }
  // select active window;
  //...
  // if none -- ask others
  //...

  return key;//cv::waitKey(delay);
}

int WaitKey( int delay )// централизованный обработчик cv::waitKey(), копит клавиши в буфере, распределяет клавиши для обработки между окнами
{
  return theFrame.waitKey(delay); //пока так
}
