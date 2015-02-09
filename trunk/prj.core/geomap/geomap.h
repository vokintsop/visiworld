#ifndef __GEOMAP_H
#define __GEOMAP_H
#include <opencv2/imgproc/imgproc.hpp> 

struct Reper
{
  cv::Point     xy;
  cv::Point2d   en;
};

struct GeoSheet  // ��������������� ���� �����
{
  std::string sheet_name; // �������� (�������������) ��� ����� �����
  cv::Mat3b raster;
  Reper a,b;

  cv::Point en2xy( cv::Point2d& en )
  {
    return cv::Point( 
      int( 0.5 + a.xy.x + (en.x-a.en.x) * (b.xy.x-a.xy.x)/(b.en.x-a.en.x) ), 
      int( 0.5 + a.xy.y + (en.y-a.en.y) * (b.xy.y-a.xy.y)/(b.en.y-a.en.y) )
      );
  }
  cv::Point2d xy2en( cv::Point& xy )
  {
    return cv::Point2d( 
      a.en.x + (xy.x-a.xy.x) * (b.en.x-a.en.x)/(b.xy.x-a.xy.x), 
      a.en.y + (xy.y-a.xy.y) * (b.en.y-a.en.y)/(b.xy.y-a.xy.y)
      );
  }

  cv::Point2d sheet_center_mercator( cv::Point2d& en ) 
    // ����������� ���������� �� en �� ������ ����� ����� � ������ �� �������� ���������
  {
    cv::Point sheet_xy_lu( 0, 0 );
    cv::Point sheet_xy_rb( raster.cols-1, raster.rows-1 );
    cv::Point2d sheet_en_lu = xy2en( sheet_xy_lu );
    cv::Point2d sheet_en_rb = xy2en( sheet_xy_rb );
    cv::Point2d sheet_en_center = (sheet_en_lu + sheet_en_rb)*0.5;

    const double radius = 6378137; // ��������� ������ ����� � ������
    double scale = cos( sheet_en_center.y * CV_PI / 180 );

    double xx = scale * radius * CV_PI * (en.x - sheet_en_center.x) / 180;
    double yy = scale * radius * log( tan( CV_PI * (en.y - sheet_en_center.y + 90. ) / 360 ) );
    return cv::Point2d( xx, yy );
  }

  GeoSheet() {}
  GeoSheet( const char* sheet_file_name ) {  create( sheet_file_name ); }
  bool create(  const char* sheet_file_name );

  inline bool read( const cv::FileNode &node );
  inline bool write( cv::FileStorage& fs );

};


class AGMTypes // supported derived AFrameObject types
{
public:
  std::vector< std::string > objTypes;
  AGMTypes()
  { // ������ �������������� ������� ��������
    // objTypes.push_back( "AFO_Unknown" );
    objTypes.push_back( "AGM_Point" );
    objTypes.push_back( "AGM_Segm" );
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
  std::string tags;
  GMObject():flags(0){}
  GMObject( ENPoint2d pt, const char* _type = "", int flags=0 ): 
    type(_type), flags(flags) 
    { 
      pts.push_back(pt); 
    }
  GMObject( ENPoint2d pt1,  ENPoint2d pt2, const char* _type = "", int flags=0 ): 
    type(_type), flags(flags) 
    { 
      pts.push_back(pt1); 
      pts.push_back(pt2); 
    }
};

class AGMObject : public GMObject
{
public:
  AGMObject( GMObject& gmo ): GMObject(gmo){}
  AGMObject( ENPoint2d pt, const char* _type = "", int flags=0 ): GMObject( pt, _type, flags ) {}
  AGMObject( ENPoint2d pt1, ENPoint2d pt2, const char* _type = "", int flags=0 ): 
    GMObject( pt1, pt2, _type, flags ) {}
  virtual ~AGMObject(){};

  virtual bool readSelf(const cv::FileNode &node) { return true; } // ���������� ����� ��������
  virtual bool writeSelf(cv::FileStorage& fs) { return true; } // ���������� ����� ��������
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } // ������ ����� ������ ���� ��������������
  virtual int getDrawThickness() { return 2; } // ������ ����� ������ ������� ����� ��������������
  virtual int getNodeRadius() { return 5; }
  virtual void draw( GeoSheet& sheet, cv::Mat& display ) 
  { // ������������� ������� ���������
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    int node_radius = getNodeRadius();
    cv::Point pt_prev;
    if (pts.size() > 0)
    {
      pt_prev = sheet.en2xy( pts[0] );
      cv::circle( display, pt_prev, getNodeRadius(), color, thickness );
    }
    for (int i=1; i< int(pts.size()); i++)
    {
      cv::Point pt = sheet.en2xy( pts[i] );
      cv::circle( display, pt, getNodeRadius(), color, thickness );
      if (i>0)
      {
        cv::line( display, pt_prev, pt, color, thickness );
        pt_prev = pt;
      }
    }
  } 
};

AGMObject* CreateAGMObject( GMObject& gmo ); //�������
AGMObject* ReadAGMObject(const cv::FileNode &node); // �������-�������� c �������������� ������������ readSelf()
bool WriteAGMObject(cv::FileStorage& fs, AGMObject* agmo ); // ������, ���������� ����������� writeSelf()


class AGM_Unknown : public AGMObject
{
public:
  AGM_Unknown( ENPoint2d pt, const char* _type = "", int flags=0 ):
    AGMObject( pt, _type, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(128,128,128); } 
  virtual int getDrawThickness() { return 3; } 
};

class AGM_Point : public AGMObject
{
public:
  AGM_Point( GMObject& gmo ): AGMObject(gmo) {}
  AGM_Point( ENPoint2d pt, const char* _type = "AGM_Point", int flags=0 ):
    AGMObject( pt, _type, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } 
  virtual int getDrawThickness() { return 2; } 
};

class AGM_Segm : public AGMObject
{
public:
  AGM_Segm( GMObject& gmo ): AGMObject(gmo) {}
  AGM_Segm( ENPoint2d pt1,  ENPoint2d pt2, const char* _type = "AGM_Segm", int flags=0 ):
    AGMObject( pt1, pt2, _type, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } 
  virtual int getDrawThickness() { return 2; } 
};

class GeoMap
{
public:
  std::string root_folder; 
  std::vector< GeoSheet > sheets;
  std::vector< cv::Ptr< AGMObject > > objects;
public:
  GeoMap(){};
  bool import( const char * _root_folder );	
  int find_best_sheet( cv::Point2d en );

  bool read( cv::FileStorage& fs );
  bool write( cv::FileStorage& fs );

  bool write();
  bool read( const char* _root_folder );

};



#endif
