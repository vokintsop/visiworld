#ifndef __GEOMAPEDITOR_H
#define __GEOMAPEDITOR_H

#include <geomap/geomap.h>

using namespace cv;

#include "ocvutils/ocvkeys.h"
#include <ocvutils/ocvgui.h>

class GeoMapEditor 
  : public OCVGuiWindow
{
  GeoMap gm;
  std::string title;
  int cur_sheet;
  cv::Point2d location;
  cv::Vec2d direction;
  int iObjType; // индекс активного типа объекта 

//  bool non_stop_mode;
public:
  void update_title();
  void update_location( cv::Point2d en, cv::Vec2d dir = cv::Vec2d(0, 1));
  void draw();
  void exportObjPoints( std::vector<cv::Point2d> &enPoints )
  {
    for (unsigned int i = 0; i < gm.objects.size(); ++i)
    {
      AGM_Point *ptPoint;
      try
      {
        ptPoint = static_cast<AGM_Point *> (gm.objects.at(i).obj);
      }
      catch(std::out_of_range &e)
      {
        std::cout << "error, GeoMap::objects subscript out of range:\n" << e.what() << std::endl;
        return;
      }
      catch(std::bad_cast &)
      {
      }
      catch(cv::Exception &e)
      {
        std::cout << "cv::Exception cought:\n" << e.what() << std::endl;
        return;
      }
      catch(std::exception &e)
      {
        std::cout << "std::exception\n" << e.what() << std::endl;
        return;
      }
      catch(...)
      {
        std::cout << "unknown exception cought\n";
      }
      enPoints.push_back(ptPoint->pts[0]);
    }
  }
public:
  GeoMapEditor( const char* sheets_list_file=NULL );
  ~GeoMapEditor()
  {
    gm.write();
  };

  virtual bool  procKey( int key ) // выполняет обработку клавиши для данного окна; возвращает true если клавиша обработана
  {
    return false; // пока не умеем обрабавать ничего
  }

  void update_window( bool quickly = false )  // _только_ здесь реальная отрисовка с отдачей управления
  {
//     if (draw_image_dirty)
//     {
//       imshow( title, draw_image );
//       draw_image_dirty = false;
//     }
    //update_title();
    update_image_to_draw();

    int key = kNoKeyPressed;
    if (pressed_keys.size() == 0)
      key = waitKey( (/*non_stop_mode ||*/ quickly) ? 1 : 0);
    if (key != kNoKeyPressed)
      std::cout << "GeoMapEditor: Key pressed " << key << std::endl;
    if (key != kNoKeyPressed && pressed_keys.size() < 1000)
      pressed_keys.push_back( key ); // отложено до processEvents()
  }

//  void drawRubbering(); // отрисовка текущего состояния вытягивания недоноска
  void update_image_to_draw()
  {
    //draw_image = frame_image.clone(); 
    draw();
    //drawObjects(); // TO DO separate
//    drawRubbering();
    update_title();
    //draw_image_dirty = true;
  }

  std::deque< int > pressed_keys; // очередь необработанных клавиш

  int processEvents();
  int processKeys();

  AGMTypes agmTypes; // текущие поддерживаемые типы объектов
  std::string objType() { return agmTypes.objTypes[iObjType]; }; // активный тип объекта 
  int processMouseEvent(int event, int x, int y, int flags);
  int finishMouseEvent(); // евент обработан, надо отрисовать и закончить его обработку

  bool rubber_by_left_button; // true если начали вытягивать _левой_ кнопкой, иначе _правой_
  //bool rubbering_rectangle; // true если в моменте вытягиваем мышкой прямоугольник
  int rubbering_mode; // какого кота мы тянем за хвост
#define RUBBERING_NOTHING 0
#define RUBBERING_RECTANGLE 1 // вытягиваем прямоугольник
#define RUBBERING_SEGMENT 2 // вытягиваем отрезок -- от точки или от последней вершины ломаной
#define RUBBERING_POLYGON 3 // уже вытянута часть многоугольника, и мы свободно водим мышкой планируя начать вытягивать следующее ребро многоугольника

//#define RUBBERING_IMAGE 3 -- например пытаемся отскроллировать картинку, на будущее
  std::vector< cv::Point > rubbering_pts; // накопленные точки

  bool addMouseObject( // пытаемся добавить новый объект вытянув или кликнув мышкой
    cv::Rect& rect, // note: in-out -- подкручиваем ректангл по законам первого рождения для данного объекта
    int flags );

  bool addMouseObject( // пытаемся добавить новый объект вытянув или кликнув мышкой
    std::vector< cv::Point >& pts, // note: in-out -- подкручиваем точки по законам первого рождения для данного объекта
    int flags );

};



#endif
