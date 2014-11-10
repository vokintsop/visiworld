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

#include "markup.h"
#include "streetglass/frameproc.h" // обработка кадра

using namespace cv;




bool MarkupWindow::addMouseObject( // пытаемся добавить новый объект для набора точек, указанных мышкой 
    std::vector< cv::Point >& pts,  // note: in-out -- подкручиваем набор точек по законам первого рождения для данного объекта
    int flags )
{
  //// correct rect:
  //if (!adjustObjectRectangle( rect, flags ))
  //{
  //  if (tracking_object)
  //    cout << "tracking object lost" << endl;
  //  return false;
  //}

  if (objType() == "AFO_Segm")
    frameProc.detect_segment( pts );

  return addObject(pts, flags);
}

bool MarkupWindow::addMouseObject( // пытаемся добавить новый объект вытянув или кликнув мышкой 
    cv::Rect& rect,  // note: in-out -- подкручиваем ректангл по законам первого рождения для данного объекта
    int flags )
{
  // correct rect:
  if (!adjustObjectRectangle( rect, flags ))
  {
    if (tracking_object)
      cout << "tracking object lost" << endl;
    return false;
  }

  return addObject(rect, flags);
}


bool MarkupWindow::trackObject( // пытаемся добавить новый объект протащив старый с прежнего кадра 
    cv::Rect& rect, // note: in-out -- подкручиваем ректангл по законам трекинга для данного объекта
    int flags )
{
  // correct rect:

  Rect target = rect;
  ////target |= 15;

  if (!adjustObjectRectangle( target, flags ))
  {
    assert(tracking_object);
    cout << "tracking object lost" << endl;
    return false;
  }
  cv::Point target_center = target.tl() + target.br(); // новый
  cv::Point rect_center = rect.tl() + rect.br(); // старый
  cv::Point shift = target_center - rect_center; // смещение центра
  cv::Point resize( target.width-rect.width, target.height-rect.height ); // изменение размера
  cout << "Tracking shift: " << shift << " resize:" << resize << endl;
  if (rect.width*0.3 + 5 < abs(resize.x) || rect.height*0.3 + 5 < abs(resize.y))  // изменение размера на 30%
  {
    assert(tracking_object);
    cout << "tracking object changed size" << endl;
    return false;
  }
  rect = target;
  return addObject(rect, flags);
}

bool MarkupWindow::addObject( // финал добавления объекта: кликнули или мышкой обвели или тречим старый объект...
  const std::vector< cv::Point >& pts // note: __in__ : на выходе ректангл объекта не меняется
  , int flags
  )
{
  // add object with specified corrected rect
  AFrameObject* new_object = CreateAFrameObject( objType(), pts );
  if (new_object == NULL)
    return false; //?
  marked_frames[iframe].objects.push_back( Ptr<AFrameObject>( new_object ) );
  int iobj = marked_frames[iframe].objects.size()-1;
  saveFrameObjectImage(iobj);
  undo.push_back( MarkupAction( MA_ADD_OBJECT, iframe, iobj  ) );

  update_image_to_draw();
  update_window(true);

  return true;
}
/////////////////////////////////////////////////////////

bool find_center( // ищем компоненту, ??? ближайшую к центру указанного прямоугольника
   cv::Rect& rect, // уточнен, если вернули true 
   CCMap& xcc, //пул компонент связности
   bool tracking_object
                 )
{
  //Rect target = rect - Point(15,15) + Size(30,30);
  int delta = 10;
  Rect target = rect - Point(delta,delta) + Size(2*delta,2*delta);
  Point center = (rect.tl() + rect.br()) * 0.5;

  double min_dist = 0x7fff0000;
  int min_i = -1;
  Rect rc_union = _InvertedRect_;
  for ( int i=1; i < int(xcc.cc.size()); i++ )
  {
    CCData& ccd = xcc.cc[i];
    // skip filtered? find nearest? todo...
    Rect rc_ccd( ccd.minx, ccd.miny, ccd.width(), ccd.height() );
    Rect rc_ex = rc_ccd - Point(15,15) + Size(30,30);
    if (rc_ex.contains( center  ))
    {
      Point center2 = (rc_ex.tl() + rc_ex.br()) * 0.5;
      center2 -= center;
      double dist = sqrt( center2.ddot( center2 ) );
      if (dist < min_dist)
      {
        min_dist = dist;
        min_i = i;
      }
    }
    if (target.contains(rc_ccd.tl()) && target.contains(rc_ccd.br())) // мишень накрывает компоненту
    {
      rc_union |= rc_ccd;
    }
  }

  if (rc_union.width > 0 && (rect.width > 5 || rect.height > 5)) // не просто кликнутая точка
  {
    rect = rc_union;
    for ( int i=1; i < int(xcc.cc.size()); i++ )
    {
      CCData& ccd = xcc.cc[i];
      Rect rc_ccd( ccd.minx, ccd.miny, ccd.width(), ccd.height() );
      if (target.contains(rc_ccd.tl()) && target.contains(rc_ccd.br())) // мишень накрывает компоненту
      {
        ccd.flags |= CC_HIDDEN;
      }
    }
     return true;
  }


  if (min_i >=0)
  {
    CCData& ccd = xcc.cc[min_i];
    // skip filtered? find nearest? todo...
    Rect rc_ccd( ccd.minx, ccd.miny, ccd.width(), ccd.height() );
    Rect rc_ex = rc_ccd - Point(15,15) + Size(30,30);
    assert( rc_ex.contains( center  ));
    rect = rc_ccd;
    ccd.flags |= CC_HIDDEN;
  }
  else /// if (!found)
    if (tracking_object || ((rect.width < 5) && (rect.height < 5)) )
      return false;

  return true;
}

bool find_inside( // обтягиваем набор прямоугольников захваченных указанным
   cv::Rect& rect, // уточнен, если вернули true 
   CCMap& xcc, //пул компонент связности
   bool tracking_object
                 )
{
  Rect target = rect;
  Rect result = _InvertedRect_;

  for ( int i=1; i < int(xcc.cc.size()); i++ )
  {
    CCData& ccd = xcc.cc[i];
    // skip filtered? find nearest? todo...
    Rect rc( ccd.minx, ccd.miny, ccd.width(), ccd.height() );

    if ( target.contains( rc.tl() ) && target.contains( rc.br() )) // захватили
    {
      result |= rc;
      ccd.flags |= CC_HIDDEN;
    }
  }
  if ( (result.width < 5) && (result.height < 5) )
      return false;
  rect = result;
  return true;
}


bool MarkupWindow::adjustObjectRectangle( 
  cv::Rect& rect // кликнули или мышкой обвели или тречим старый объект... -- хотим уточнить прямоугольник
  , int flags
  )
{
  if (objType() == "AFO_Rect" || objType() == "AFO_Point" || objType() == "AFO_Segm" )
    return true;

  CCMap& xcc = 
    (objType() == "AFO_RedManOverTimer" || objType() == "AFO_RedManSingle" || objType() == "AFO_RedStopLights")     ? frameProc.redcc :
    (objType() == "AFO_GreenManOverTimer" || objType() == "AFO_GreenManSingle") ? frameProc.grecc :
    //(objType() == "AFO_BlueSquare") 
    frameProc.blucc;

  if ( flags & ( ADD_OBJECT_MOUSE_LEFT | ADD_OBJECT_TRACKED ) ) // кликнули левой -- ищем в кого ближайший центр
    if (!find_center(rect, xcc, tracking_object))
      return false;

  if (flags & ADD_OBJECT_MOUSE_RIGHT) // обтянули правой -- кого полностью покрыли
    if (!find_inside(rect, xcc, tracking_object))
      return false;

  return true;
}
