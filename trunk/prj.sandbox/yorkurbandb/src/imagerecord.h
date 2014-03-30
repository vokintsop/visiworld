#ifndef __IMAGERECORD_H
#define __IMAGERECORD_H

#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class ImageRecord // результаты по картинке 
{
public:
  // входные данные 
  std::string name; // полное (от корня) имя картинки, без расширения, например, "//testdata/yorkurbandb/P1020171/P1020171"
  int camera_foo; // что то про камеру

  // рабочие данные по текущей картинке: 
  std::vector< std::pair< cv::Point, cv::Point > > segments; // отрезки, выделенные на текущем изображении
  std::vector< cv::Point3f > lines; // отрезки, преобразованные к линиям (с учетом параметров камеры)
  std::vector< cv::Point3f > vanish_points; // точки схода (с учетом параметров камеры)
  void explore();
  
  // на выход
  int results_bee; // результаты обработки картинки для подсчета статистики
};


bool read_image_records( std::string& root, std::vector< ImageRecord >& image_records );
void make_report( std::vector< ImageRecord >& image_records );

#endif // __IMAGERECORD_H