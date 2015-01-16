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
  int iObjType; // ������ ��������� ���� ������� 
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

  virtual bool  procKey( int key ) // ��������� ��������� ������� ��� ������� ����; ���������� true ���� ������� ����������
  {
    return false; // ���� �� ����� ���������� ������
  }


  AGMTypes agmTypes; // ������� �������������� ���� ��������
  std::string objType() { return agmTypes.objTypes[iObjType]; }; // �������� ��� ������� 
  int processMouseEvent(int event, int x, int y, int flags);
  int finishMouseEvent(); // ����� ���������, ���� ���������� � ��������� ��� ���������

  bool rubber_by_left_button; // true ���� ������ ���������� _�����_ �������, ����� _������_
  //bool rubbering_rectangle; // true ���� � ������� ���������� ������ �������������
  int rubbering_mode; // ������ ���� �� ����� �� �����
#define RUBBERING_NOTHING 0
#define RUBBERING_RECTANGLE 1 // ���������� �������������
#define RUBBERING_SEGMENT 2 // ���������� ������� -- �� ����� ��� �� ��������� ������� �������
#define RUBBERING_POLYGON 3 // ��� �������� ����� ��������������, � �� �������� ����� ������ �������� ������ ���������� ��������� ����� ��������������

//#define RUBBERING_IMAGE 3 -- �������� �������� ��������������� ��������, �� �������
  std::vector< cv::Point > rubbering_pts; // ����������� �����

  bool addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������
    cv::Rect& rect, // note: in-out -- ������������ �������� �� ������� ������� �������� ��� ������� �������
    int flags );

  bool addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������
    std::vector< cv::Point >& pts, // note: in-out -- ������������ ����� �� ������� ������� �������� ��� ������� �������
    int flags );

};



#endif
