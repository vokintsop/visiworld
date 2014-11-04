#ifndef _COREPROC_H
#define _COREPROC_H

//////////////////////////////// 
#include <deque>
template < class T > 
class history // ������ � ����������� ������� ���������� � ������
{
  std::deque< T > base; // ������� ��� ��������
  int deleted; // ������� ��������� ������ -- ��� ��������� ����������� �������
public:
  history():deleted(0){}
  T& operator [] ( int i ) { return base[i-deleted]; }
  void push_back( T& t ) { base.push_back(t); }
  void pop_front() { base.pop_front(); deleted++; }
};
///////////////////////////////////

#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "frameproc.h"

// ��������. ��� ����� ��� �� ����� �������� ���������

class CoreState // ���������: �������� - ���� ��������, ���� ��������, ���������...
{
public:
  double go; //  ����������� ����������� ������ �� ������� � ������� 
  CoreState(): go(0.5) {}
  

#define CP_STATE_UNKNOWN 0
#define CP_STATE_STOP 1 // � ������ ����� ������� ������
#define CP_STATE_GO 2 // � ������ ����� ������� ������
  std::string dummy;
};

class CoreEvent // ��������������� �������. ��������, ������� � ����� ���������. �������� ������������.
{
  int time;
  std::string dummy;
};

//??
//class CoreInput // ������� ������, ��������� ������ ������� ������
//{
//  int time;
//  cv::Mat frame;
//  //cv::Ptr< cv::Point3f > location;
//  //std::string dummy;
//};

class CoreOutput // �������� ������
{
  std::string dummy;
};

class CoreProc // ������� ���� ���������
{
  history< CoreEvent > events; // ������� (��������, ����������)
  history< FrameProc > frameprocs; // ��������� ������������ �����
  history< CoreOutput > pendingoutput; // ���������, ��������� �������� ������
public:
  cv::Mat display; // �������� ��� ���������
  bool process( cv::Mat& input_frame );

  CoreState last_state; // ��������� ����� process


  //??bool process( CoreInput& input );

}; // class CoreProc

#endif // __COREPROC_H