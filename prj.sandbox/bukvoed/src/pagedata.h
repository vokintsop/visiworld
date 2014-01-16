// pagedata.h
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>

#include "ccdata.h"

struct PageData
{
  // input
  std::string source_filename;
  int source_frame_index;

  // рабочие битмапы
  cv::Mat1b src; // входное изображение, ч-б, 0-черное (сигнал), 255-белое (фон)
  cv::Mat1b src_dilated; // размазанное входное
  cv::Mat1b src_binarized; // 0-background, 255-foreground

  // компоненты связности
  cv::Mat1i labels; // карта компонент связности, [2...labels.size()-1] ==> cc[]
  std::vector< CCData > cc; // информация о компонентах
  // настройки препроцессинга
  cv::Rect ROI;

  PageData(){}
  PageData( const char* filename ) {    compute(filename);  }
  bool compute( const char* filename );
};