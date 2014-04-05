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
  void report( const char* prefix_message = "",   const char* suffix_message = " msec" ) // 
  {
#ifndef NO_TICKER_MESSAGES
    cout <<  prefix_message << msecs() << suffix_message << endl;
#endif
  }


};

#endif // __TICKER_H