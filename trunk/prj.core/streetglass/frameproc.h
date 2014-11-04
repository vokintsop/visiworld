#ifndef __FRAMEPROC_H
#define __FRAMEPROC_H

#include "conncomp/ccmap.h"
#include "streetglass/sticks.h"

class FrameProc  // ������������� ������, ������������� � �������� ������������� �����
{
public:
  cv::Mat3b bgr720; // �������� ������ 1280�720 (���� ������� ������� �������, �������� FullHD -- ���������� � ������ �������)
  cv::Mat3b bgr360; // bgr1024 ������ � 2 ����

#define FP_REDCC 0x1
#define FP_BLUCC 0x2
#define FP_GRECC 0x4
#define FP_STICKS 0x8

#define FP_ALL 0xffff

  bool FrameProc::process( cv::Mat& input_bgr720, int scheme = FP_ALL ); // ���������� �������� ������� ��������
  bool FrameProc::draw( cv::Mat& display, const std::string& objType ); // ������������

  // ������� 1
  bool FrameProc::compute_binmask( int scheme );
  cv::Mat1b blumask; // �������������� �������� ����� ����� (���� ����������� ��������, ��������)
  cv::Mat1b gremask; // �������������� �������� ������� ����� (������� ��������)
  cv::Mat1b redmask; // �������������� �������� ������� ��������
  bool FrameProc::compute_cc();
  CCMap blucc; // ����� ���������� ���������
  CCMap grecc; // ������� ���������� ���������
  CCMap redcc; // ������� ���������� ���������

  cv::Mat1b vsticks1b; // ����� ������������ �����

  bool detect_segment( std::vector< cv::Point >& pts ); // ����������� ��������� ������� � ������� ��������� ����� ��� �������

};


#endif // __FRAMEPROC_H