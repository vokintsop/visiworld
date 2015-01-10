#ifndef __GEOMAPEDITOR_H
#define __GEOMAPEDITOR_H

#include <geomap/geomap.h>

class GeoMapEditor
{
  GeoMap gm;
  string title;
  int cur_sheet;
  Point2d location;
  int iObjType; // ������ ��������� ���� ������� 
public:
  void GeoMapEditor::update_title();
  void update_location( cv::Point2d en );
  void draw();
public:
  GeoMapEditor( const char* sheets_list_file=NULL );
  ~GeoMapEditor()
  {
    gm.write();
  };

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