#ifndef __GEOMAPEDITOR_H
#define __GEOMAPEDITOR_H

#include <geomap/geomap.h>
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
        cout << "error, GeoMap::objects subscript out of range:\n" << e.what() << endl;
        return;
      }
      catch(std::bad_cast &)
      {
      }
      catch(cv::Exception &e)
      {
        cout << "cv::Exception cought:\n" << e.what() << endl;
        return;
      }
      catch(std::exception &e)
      {
        cout << "std::exception\n" << e.what() << endl;
        return;
      }
      catch(...)
      {
        cout << "unknown exception cought\n";
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

  virtual bool  procKey( int key ) // выполн€ет обработку клавиши дл€ данного окна; возвращает true если клавиша обработана
  {
    return false; // пока не умеем обрабавать ничего
  }


  AGMTypes agmTypes; // текущие поддерживаемые типы объектов
  std::string objType() { return agmTypes.objTypes[iObjType]; }; // активный тип объекта 
  int processMouseEvent(int event, int x, int y, int flags);
  int finishMouseEvent(); // евент обработан, надо отрисовать и закончить его обработку

  bool rubber_by_left_button; // true если начали выт€гивать _левой_ кнопкой, иначе _правой_
  //bool rubbering_rectangle; // true если в моменте выт€гиваем мышкой пр€моугольник
  int rubbering_mode; // какого кота мы т€нем за хвост
#define RUBBERING_NOTHING 0
#define RUBBERING_RECTANGLE 1 // выт€гиваем пр€моугольник
#define RUBBERING_SEGMENT 2 // выт€гиваем отрезок -- от точки или от последней вершины ломаной
#define RUBBERING_POLYGON 3 // уже выт€нута часть многоугольника, и мы свободно водим мышкой планиру€ начать выт€гивать следующее ребро многоугольника

//#define RUBBERING_IMAGE 3 -- например пытаемс€ отскроллировать картинку, на будущее
  std::vector< cv::Point > rubbering_pts; // накопленные точки

  bool addMouseObject( // пытаемс€ добавить новый объект выт€нув или кликнув мышкой
    cv::Rect& rect, // note: in-out -- подкручиваем ректангл по законам первого рождени€ дл€ данного объекта
    int flags );

  bool addMouseObject( // пытаемс€ добавить новый объект выт€нув или кликнув мышкой
    std::vector< cv::Point >& pts, // note: in-out -- подкручиваем точки по законам первого рождени€ дл€ данного объекта
    int flags );

};



#endif
