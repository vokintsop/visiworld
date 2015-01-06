#ifndef __GEOMAP_H
#define __GEOMAP_H

struct Reper
{
  cv::Point     xy;
  cv::Point2d   en;
};

struct GeoSheet  // топографический лист карты
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

struct GMObject // GeoMap Object. ќбъект, отмеченный на карте
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
  int iObjType; // индекс активного типа объекта 
public:
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

  bool GeoMap::addMouseObject( // пытаемс€ добавить новый объект выт€нув или кликнув мышкой
    std::vector< cv::Point >& pts, // note: in-out -- подкручиваем точки по законам первого рождени€ дл€ данного объекта
    int flags );

};



#endif