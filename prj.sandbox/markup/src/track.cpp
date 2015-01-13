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

#include "streetglass/framedata.h" // данные, распознанные или размеченные на кадре

using namespace cv;

//надо здесь исправить video;
int trackRectangle( cv::VideoCapture& video, cv::Rect& rect, int nframes, 
          std::vector< cv::Rect >& result ) 
// начиная с текущей позиции в video тречим rect на nframes вперед или назад (отрицательное nframes)
// показываем результат и возвращаем клавишу подтверждения/отказа или какую-то еще
{
  cout << "Start tracking rectangle..."  << endl;
  return 0; // tmp
}