#ifndef __GEOMAP_H
#define __GEOMAP_H

struct Reper
{
  cv::Point     xy;
  cv::Point2d   en;
};

struct GeoSheet  // ��������������� ���� �����
{
  cv::Mat3b raster;
  Reper a,b;

  cv::Point en2xy( cv::Point2d& en )
  {
    return Point( 
      int( 0.5 + a.xy.x + (en.x-a.en.x) * (b.xy.x-a.xy.x)/(b.en.x-a.en.x) ), 
      int( 0.5 + a.xy.y + (en.y-a.en.y) * (b.xy.y-a.xy.y)/(b.en.y-a.en.y) )
      );
  }
  cv::Point2d xy2en( cv::Point& xy )
  {
    return Point2d( 
      a.en.x + (xy.x-a.xy.x) * (b.en.x-a.en.x)/(b.xy.x-a.xy.x), 
      a.en.y + (xy.y-a.xy.y) * (b.en.y-a.en.y)/(b.xy.y-a.xy.y)
      );
  }

  GeoSheet() {}
  GeoSheet( const char* sheet_file_name ) {  read( sheet_file_name ); }
  bool read(  const char* sheet_file_name );

};


class AGMTypes // supported derived AFrameObject types
{
public:
  std::vector< string > objTypes;
  AGMTypes()
  { // ������ �������������� ������� ��������
    // objTypes.push_back( "AFO_Unknown" );
    objTypes.push_back( "AFO_Point" );
    // objTypes.push_back( "AFO_Segm" );
    //////objTypes.push_back( "AFO_Rect" );
    //////objTypes.push_back( "AFO_Quad" );
    //////objTypes.push_back( "AFO_Triangle" );


    //////objTypes.push_back( "AFO_RedStopLights" );
    //////objTypes.push_back( "AFO_RedManSingle" );
    //////objTypes.push_back( "AFO_GreenManSingle" );
    ////////objTypes.push_back( "AFO_RedManOverTimer" );
    //////objTypes.push_back( "AFO_GreenManBelowTimer" );
    //////objTypes.push_back( "AFO_BlueSquare" );
  }
};

typedef cv::Point2d ENPoint2d; // ����� � ����������� EN � ������� ��������� (x=east, y=nord) 
typedef cv::Point3d ENUPoint3d; // ����� � ����������� ENU � ������� ��������� (x=east, y=nord, z=up)

struct GMObject // GeoMap Object. ������, ���������� �� �����
{
  std::string type;
  std::vector< ENPoint2d > pts;  // ����� �������� ����� � ����������� EN (ENU?)
  int flags;
  GMObject( ENPoint2d pt, const char* _type = "", int flags=0 ): type(_type), flags(flags) { pts.push_back(pt); }
};

class AGeoMapObject : public GMObject
{
public:
  AGeoMapObject( ENPoint2d pt, const char* _type = "", int flags=0 ): GMObject( pt, _type, flags ) {}
  virtual ~AGeoMapObject(){};
  virtual bool readSelf(cv::FileNode &node) { return true; }
  virtual bool writeSelf(cv::FileStorage& fs) { return true; };
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } // ������ ����� ������ ���� ��������������
  virtual int getDrawThickness() { return 2; } // ������ ����� ������ ������� ����� ��������������
  virtual int getNodeRadius() { return 5; }
  virtual void draw( GeoSheet& sheet, Mat& display ) 
  { // ������������� ������� ���������
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    int node_radius = getNodeRadius();
    Point pt_prev;
    for (int i=0; i< int(pts.size()); i++)
    {
      Point pt = sheet.en2xy( pts[i] );
      cv::circle( display, pt, getNodeRadius(), color, thickness );
      if (i>0)
      {
        cv::line( display, pt_prev, pt, color, thickness );
        pt_prev = pt;
      }
    }
  } 
};

class AGM_Unknown : public AGeoMapObject
{
public:
  AGM_Unknown( ENPoint2d pt, const char* _type = "", int flags=0 ):
    AGeoMapObject( pt, _type, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(128,128,128); } 
  virtual int getDrawThickness() { return 3; } 
};

class AGM_Point : public AGeoMapObject
{
public:
  AGM_Point( ENPoint2d pt, const char* _type = "", int flags=0 ):
    AGeoMapObject( pt, _type, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } 
  virtual int getDrawThickness() { return 2; } 
};

class GeoMap
{
public:
  std::vector< GeoSheet > sheets;
  int cur_sheet;
  std::vector< Point2d > enpoints;
  Point2d location;
public:
  string title;
  GeoMap();
  GeoMap( const char* sheets_list_file );
  bool open(  const char* sheets_list_file  );	
  int find_best_sheet( Point2d en );
  void update_location( cv::Point2d en );
  void draw();

//////////////////// GeoMapEditor ??

/////////////////////////// supported object types
  int iObjType; // ������ ��������� ���� ������� 
public:
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

  bool GeoMap::addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������
    std::vector< cv::Point >& pts, // note: in-out -- ������������ ����� �� ������� ������� �������� ��� ������� �������
    int flags );

};



#endif