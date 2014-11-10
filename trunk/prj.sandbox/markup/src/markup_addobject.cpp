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
#include "streetglass/frameproc.h" // ��������� �����

using namespace cv;




bool MarkupWindow::addMouseObject( // �������� �������� ����� ������ ��� ������ �����, ��������� ������ 
    std::vector< cv::Point >& pts,  // note: in-out -- ������������ ����� ����� �� ������� ������� �������� ��� ������� �������
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

bool MarkupWindow::addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������ 
    cv::Rect& rect,  // note: in-out -- ������������ �������� �� ������� ������� �������� ��� ������� �������
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


bool MarkupWindow::trackObject( // �������� �������� ����� ������ �������� ������ � �������� ����� 
    cv::Rect& rect, // note: in-out -- ������������ �������� �� ������� �������� ��� ������� �������
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
  cv::Point target_center = target.tl() + target.br(); // �����
  cv::Point rect_center = rect.tl() + rect.br(); // ������
  cv::Point shift = target_center - rect_center; // �������� ������
  cv::Point resize( target.width-rect.width, target.height-rect.height ); // ��������� �������
  cout << "Tracking shift: " << shift << " resize:" << resize << endl;
  if (rect.width*0.3 + 5 < abs(resize.x) || rect.height*0.3 + 5 < abs(resize.y))  // ��������� ������� �� 30%
  {
    assert(tracking_object);
    cout << "tracking object changed size" << endl;
    return false;
  }
  rect = target;
  return addObject(rect, flags);
}

bool MarkupWindow::addObject( // ����� ���������� �������: �������� ��� ������ ������ ��� ������ ������ ������...
  const std::vector< cv::Point >& pts // note: __in__ : �� ������ �������� ������� �� ��������
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

bool find_center( // ���� ����������, ??? ��������� � ������ ���������� ��������������
   cv::Rect& rect, // �������, ���� ������� true 
   CCMap& xcc, //��� ��������� ���������
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
    if (target.contains(rc_ccd.tl()) && target.contains(rc_ccd.br())) // ������ ��������� ����������
    {
      rc_union |= rc_ccd;
    }
  }

  if (rc_union.width > 0 && (rect.width > 5 || rect.height > 5)) // �� ������ ��������� �����
  {
    rect = rc_union;
    for ( int i=1; i < int(xcc.cc.size()); i++ )
    {
      CCData& ccd = xcc.cc[i];
      Rect rc_ccd( ccd.minx, ccd.miny, ccd.width(), ccd.height() );
      if (target.contains(rc_ccd.tl()) && target.contains(rc_ccd.br())) // ������ ��������� ����������
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

bool find_inside( // ���������� ����� ��������������� ����������� ���������
   cv::Rect& rect, // �������, ���� ������� true 
   CCMap& xcc, //��� ��������� ���������
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

    if ( target.contains( rc.tl() ) && target.contains( rc.br() )) // ���������
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
  cv::Rect& rect // �������� ��� ������ ������ ��� ������ ������ ������... -- ����� �������� �������������
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

  if ( flags & ( ADD_OBJECT_MOUSE_LEFT | ADD_OBJECT_TRACKED ) ) // �������� ����� -- ���� � ���� ��������� �����
    if (!find_center(rect, xcc, tracking_object))
      return false;

  if (flags & ADD_OBJECT_MOUSE_RIGHT) // �������� ������ -- ���� ��������� �������
    if (!find_inside(rect, xcc, tracking_object))
      return false;

  return true;
}
