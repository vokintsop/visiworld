// mainframe.cpp -- ������� ���� � ��
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

int WaitKey( int delay )// ���������������� ���������� cv::waitKey(), ����� ������� � ������, ������������ ������� ��� ��������� ����� ������
{
  return theFrame.waitKey(delay); //���� ���
}
