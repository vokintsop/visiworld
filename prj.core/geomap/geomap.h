#ifndef __GEOMAP_H
#define __GEOMAP_H

struct Reper
{
  cv::Point     xy;
  cv::Point2d   en;
};

struct GeoSheet  // топографический лист карты
{
  std::string sheet_name; // короткое (относительное) имя файла листа
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
  GeoSheet( const char* sheet_file_name ) {  create( sheet_file_name ); }
  bool create(  const char* sheet_file_name );

  inline bool read( cv::FileNode &node );
  inline bool write( cv::FileStorage& fs );

};


class AGMTypes // supported derived AFrameObject types
{
public:
  std::vector< string > objTypes;
  AGMTypes()
  { // список поддерживаемых классов объектов
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

typedef cv::Point2d ENPoint2d; // точка в координатах EN с двойной точностью (x=east, y=nord) 
typedef cv::Point3d ENUPoint3d; // точка в координатах ENU с двойной точностью (x=east, y=nord, z=up)

struct GMObject // GeoMap Object. Объект, отмеченный на карте
{
  std::string type;
  std::vector< ENPoint2d > pts;  // набор ключевых точек в координатах EN (ENU?)
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
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } // иногда проще только цвет переопределить
  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
  virtual int getNodeRadius() { return 5; }
  virtual void draw( GeoSheet& sheet, Mat& display ) 
  { // умолчательный вариант отрисовки
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
  string root_folder; 
  std::vector< GeoSheet > sheets;
  std::vector< Point2d > objects;
public:
  GeoMap(){};
  bool import( const char * _root_folder );	
  int find_best_sheet( Point2d en );

  bool read( cv::FileNode &node );
  bool write( cv::FileStorage& fs );

  bool write();
  bool read( const char* _root_folder );

};

inline bool GeoSheet::write( cv::FileStorage& fs )
{
  fs << "{:";
  fs << "sheet_name"    << sheet_name;
  fs << "reper_a_xy" << a.xy;
  fs << "reper_a_en" << a.en;
  fs << "reper_b_xy" << b.xy;
  fs << "reper_b_en" << b.en;
  fs << "}";
  return true; // todo __try
}

inline bool GeoMap::read( cv::FileNode &node )
{
  if (!node.empty())
  {
    cv::FileNode sheets_node = node["GeoMapSheets"];
    for (cv::FileNodeIterator it = sheets_node.begin(); it != sheets_node.end(); ++it)
    {
      GeoSheet sh; 
      if (!sh.read(*it))
        return __false("Cannot read GeoMap sheet");
      sheets.push_back( sh );
    }
  }
  return true;
}

inline bool GeoMap::write( cv::FileStorage& fs )
{
  fs << "GeoMapSheets" << "[";
  for (int i=0; i<int(sheets.size()); i++)
    if (!sheets[i].write( fs ))
      return __false("Can't write GeoMap sheet");
  fs << "]";

  //fs << "GeoMapObjects" << "[";
  //for (int i=0; i<int(objects.size()); i++)
  //  objects[i].write( fs );
  //fs << "]";

  return true;
}

#endif