#ifndef __TICKER_H
#define __TICKER_H

// opencv:
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

typedef long long msec; // ����� � ������������� ������������� 
typedef double dsec; // ����� � double-�������� 

class Ticker // �������� �������
{
  int64 start;
public:
  Ticker() { reset(); }
  int64 reset() { start = cv::getTickCount(); return start; } // ����� �������
  int64 ticks() { return cv::getTickCount()-start; } // ����� ����� ������ ������� � �����
  dsec  dsecs() { return dsec(ticks()) / cv::getTickFrequency(); } // ����� ����� ������ ������� � �������� (double)
  msec  msecs() { return msec( (1000*ticks()) / cv::getTickFrequency() ); } // ����� ����� ������ ������� � ������������� �������������
};

#endif // __TICKER_H