#ifndef __FRAMEPROC_H
#define __FRAMEPROC_H

#include "conncomp/ccmap.h"
#include "streetglass/sticks.h"

class FrameProc  // промежуточные данные, накапливаемые в процессе распознавания кадра
{
public:
  double sensitivity; // в зависимости от уровня разменивает ошибки первого рода на ошибки второго рода
  bool detailed_visualization; // режим подробного показа


  FrameProc(): sensitivity(1.), detailed_visualization(false) 
  {}

  cv::Mat3b bgr720; // исходный битмап 1280х720 (если входной другого формата, например FullHD -- приводится к такому размеру)
  cv::Mat3b bgr360; // bgr1024 сжатый в 2 раза

#define FP_REDCC 0x1
#define FP_BLUCC 0x2
#define FP_GRECC 0x4
#define FP_STICKS 0x8

#define FP_NONE 0
#define FP_ALL 0xffff

  bool process( cv::Mat& input_bgr720,
    //int scheme = FP_ALL ); // подготовка основных рабочих битмапов
    int scheme = FP_NONE ); // подготовка основных рабочих битмапов
  bool draw( cv::Mat& display, const std::string& objType ); // визуализация

  // вариант 1
  bool compute_binmask( int scheme );
  cv::Mat1b blumask; // бинаризованная картинка синих пятен (знак пешеходного перехода, например)
  cv::Mat1b gremask; // бинаризованная картинка зеленых пятен (зеленый светофор)
  cv::Mat1b redmask; // бинаризованная картинка красных огоньков
  bool compute_cc();
  CCMap blucc; // синие компоненты связности
  CCMap grecc; // зеленые компоненты связности
  CCMap redcc; // красные компоненты связности

  cv::Mat1b vsticks1b; // карта вертикальных палок

  bool detect_segment( std::vector< cv::Point >& pts ); // детектирует наилучший сегмент в области указанной точки или отрезка

};


#endif // __FRAMEPROC_H
