#ifndef __IMAGERECORD_H
#define __IMAGERECORD_H

#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ocvutils/hcoords.h"

class ImageRecord // ���������� �� �������� 
{
public:
  ImageRecord(): hcoords(640, 480), how_to_use(0) {};
  // ������� ������ 
  std::string name; // ������ (�� �����) ��� ��������, ��� ����������, ��������, "//testdata/yorkurbandb/P1020171/P1020171"
  /// ???? int camera_foo; // ��� �� ��� ������
  HCoords hcoords; // ��������� ��������� ������

  // ground truth
  cv::Point3d truth[3]; // ���������� �������� ����� �����
  cv::Point3d truth_ort[3]; // �������������������� ���������� �������� ����� �����
  double sigma[3];

  // ������� ������ �� ������� ��������: 
  std::vector< std::pair< cv::Point, cv::Point > > segments; // �������, ���������� �� ������� �����������
  std::vector< cv::Point3d > lines; // �������, ��������������� � ������ (� ������ ���������� ������)
  std::vector< cv::Point3d > vanish_points; // ����� ����� (� ������ ���������� ������)
  void explore();
  
  int how_to_use; // 0 - test, 1 - train, 2+ - reserved 

  // �� �����
  int results_bee; // ���������� ��������� �������� ��� �������� ����������
};


bool read_image_records( std::string& root, std::vector< ImageRecord >& image_records );
void make_report( std::vector< ImageRecord >& image_records );

#endif // __IMAGERECORD_H