#include <fstream>
#include <string>
#include <algorithm>
#include <cassert>

#include <iostream>
#include <stdio.h>
using namespace std;

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "streetglass/framedata.h" // ������, ������������ ��� ����������� �� �����

using namespace cv;

//���� ����� ��������� video;
int trackRectangle( cv::VideoCapture& video, cv::Rect& rect, int nframes, 
          std::vector< cv::Rect >& result ) 
// ������� � ������� ������� � video ������ rect �� nframes ������ ��� ����� (������������� nframes)
// ���������� ��������� � ���������� ������� �������������/������ ��� �����-�� ���
{
  cout << "Start tracking rectangle..."  << endl;
  return 0; // tmp
}