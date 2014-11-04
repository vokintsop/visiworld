#ifndef _COREPROC_H
#define _COREPROC_H

//////////////////////////////// 
#include <deque>
template < class T > 
class history // вектор с затираемыми старыми элементами с начала
{
  std::deque< T > base; // базовый дек объектов
  int deleted; // счетчик удаленных старых -- для поддержки глобального индекса
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

// набросок. нам нужно что то вроде автомата соорудить

class CoreState // состояние: например - ищем светофор, ждем зеленого, переходим...
{
public:
  double go; //  независимые накопленные оценки по красным и зеленым 
  CoreState(): go(0.5) {}
  

#define CP_STATE_UNKNOWN 0
#define CP_STATE_STOP 1 // в центре очков красный сигнал
#define CP_STATE_GO 2 // в центре очков зеленый сигнал
  std::string dummy;
};

class CoreEvent // протоколируемое событие. например, переход в новое состояние. светофор переключился.
{
  int time;
  std::string dummy;
};

//??
//class CoreInput // входной сигнал, очередная порция входных данных
//{
//  int time;
//  cv::Mat frame;
//  //cv::Ptr< cv::Point3f > location;
//  //std::string dummy;
//};

class CoreOutput // выходной сигнал
{
  std::string dummy;
};

class CoreProc // главный цикл обработки
{
  history< CoreEvent > events; // события (возможно, внутренние)
  history< FrameProc > frameprocs; // последние обработанные кадры
  history< CoreOutput > pendingoutput; // сообщения, ожидающие отгрузки наружу
public:
  cv::Mat display; // картинка для отрисовки
  bool process( cv::Mat& input_frame );

  CoreState last_state; // состояние после process


  //??bool process( CoreInput& input );

}; // class CoreProc

#endif // __COREPROC_H