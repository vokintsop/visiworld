#ifndef __IMAGERECORD_H
#define __IMAGERECORD_H

#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ocvutils/hcoords.h"

typedef std::pair< cv::Point, cv::Point > Segment; // отрезок в координатах изображения
typedef cv::Point3d HLine3d; // линия в однородных координатах


class ImageRecord // результаты по картинке 
{
public:
  ImageRecord(): hcoords(640, 480), how_to_use(0) {};
  // входные данные 
  std::string name; // полное (от корня) имя картинки, без расширения, например, "//testdata/yorkurbandb/P1020171/P1020171"
  HCoords hcoords; // конвертер координат камеры

  // ground truth
  cv::Point3d truth[3]; // правильные значения точек схода
  cv::Point3d truth_ort[3]; // ортогонализированные правильные значения точек схода
  double sigma[3];

  // рабочие данные по текущей картинке: 
  std::vector< Segment > segments; // отрезки, выделенные на текущем изображении
  std::vector< HLine3d > hlines; // отрезки, преобразованные к линиям (с учетом параметров камеры)
  std::vector< cv::Point3d > vanish_points; // точки схода (с учетом параметров камеры)
  void explore();
  
  int how_to_use; // 0 - test, 1 - train, 2+ - reserved 

  // на выход
  int results_bee; // результаты обработки картинки для подсчета статистики
};


bool read_image_records( std::string& root, std::vector< ImageRecord >& image_records );
void make_report( std::vector< ImageRecord >& image_records );

#endif // __IMAGERECORD_H