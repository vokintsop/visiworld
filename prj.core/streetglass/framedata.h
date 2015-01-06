#ifndef __FRAMEDATA_H
#define __FRAMEDATA_H

#include <vector>
#include <string>
#include <ocvutils/ocvutils.h>

struct FrameObject // объект, распознанный или размеченный на кадре
{
  std::string type; // может лучше тут const char* ?? 
  std::string name; // "cc379" уникально идентифицирует в рамках кадра????
  std::string tags; // #track:6485-cc467-6497-cc247 
	cv::Rect rect;   // прямоугольник в координатах кадра
  int flags;
#define FO_FLAGS_SMALL 0x0001 // объект маленький и не обязательно должен быть детектирован (вдалеке)
// пометки по текущему состоянию в markup
#define FO_FLAGS_SELECTED 0x00010000 // объект селектирован
#define FO_FLAGS_DELETED 0x00020000 // объект помечен как удаленный

	std::string value; 	
      // -2 объект 
      // -1 неопределенное значение, иначе связанное с типом числовое значение, 
			// для светофоров -- распознанное значение числа

  FrameObject( string type = "None", cv::Rect rect=cv::Rect(0,0,0,0), int flags=0, string value = "" ): 
   type(type), rect(rect), flags(flags), value(value) {}
  //void setType( int _type ) { type = _type; }
  //void setVal( int _val ) { val = _val; }
};

/////////////////////////////////////////////////
class AFrameObject : public FrameObject
{
public:
  AFrameObject(  const string& type, cv::Rect rect=cv::Rect(0,0,0,0), int flags=0, string value = "" ):
      FrameObject( type, rect, flags, value ) {}

  virtual ~AFrameObject(){}
  virtual bool readSelf(cv::FileNode &node) { return true; }
  virtual bool writeSelf(cv::FileStorage& fs) { return true; };
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } // иногда проще только цвет переопределить
  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::Rect& r = rect;
    cv::rectangle( display, r, color, thickness );
    cv::Point lu( r.x, r.y );
    cv::Point rb( r.x+r.width-1, r.y+r.height-1 ); 
    cv::Point ru( r.x+r.width-1, r.y );
    cv::Point lb( r.x, r.y+r.height-1 );

    cv::line( display, lu, rb, color, thickness );
    cv::line( display, ru, lb, color, thickness );
  }
  //virtual 
  //virtual cv::Size minSize() { return Size(0,0); }
  //virtual cv::Size maxSize() { return Size(0,0); }
};


class AFO_Unknown : public AFrameObject
{
public:
  AFO_Unknown( std::string type, cv::Rect rect, int flags ):
    AFrameObject( type, rect, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(128,128,128); } 
  virtual int getDrawThickness() { return 1; } 

};

class AFO_Point : public AFrameObject  // точка "общего назначения"
{
public:
  AFO_Point( std::string type, cv::Point pnt, int flags ):
      AFrameObject( type, cv::Rect(pnt.x, pnt.y, 0, 0), flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } 
  virtual int getDrawThickness() { return 2; } 
  virtual void draw( cv::Mat& display ) // Х
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    int d=5;
    cv::Rect& r = rect;
    cv::Point lu( r.x-d, r.y-d );
    cv::Point rb( r.x+d, r.y+d ); 
    cv::Point ru( r.x+d, r.y-d );
    cv::Point lb( r.x-d, r.y+d );  

    cv::line( display, lu, rb, color, thickness );
    cv::line( display, ru, lb, color, thickness );
  }

};

class AFO_Polygon : public AFrameObject  // многоугольник "общего назначения"
{
public:
  std::vector<cv::Point> pts;
public:
  AFO_Polygon( std::string type, const std::vector<cv::Point>& _pts, int flags ):
    AFrameObject( type, bounding(pts), flags ), pts(_pts)
    {
      if (pts.size() > 1 && pts[0] == pts.back())
        pts.pop_back();
    }
  AFO_Polygon( std::string type, cv::Rect rect, int flags ):
    AFrameObject( type, rect, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } 
  virtual int getDrawThickness() { return 2; } 
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    int n = pts.size();
    for (int i=0; i<n; i++)
    {
      cv::line( display, pts[i], pts[(i+1)%n ], color, thickness );
      cv::circle( display, pts[i], 5, color, thickness );
    }
  }
  virtual bool readSelf(cv::FileNode &node) 
  { 
    node["vertices"] >> pts;
    return true; 
  }
  virtual bool writeSelf(cv::FileStorage& fs) 
  { 
    fs << "vertices" << pts;
    return true; 
  };

};

class AFO_Segm : public AFrameObject  // отрезок "общего назначения"
{
  cv::Point u,v;
public:
  AFO_Segm( std::string type, cv::Point u, cv::Point v, int flags ):
      AFrameObject( type, cv::Rect(u, v), flags ), u(u), v(v)
    {}
  AFO_Segm( std::string type, cv::Rect rect, int flags ):
      AFrameObject( type, rect, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } 
  virtual int getDrawThickness() { return 2; } 
  virtual void draw( cv::Mat& display ) // Х
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::line( display, u, v, color, thickness );
    cv::circle( display, u, 5, color, thickness );
    cv::circle( display, v, 5, color, thickness );
  }
  virtual bool readSelf(cv::FileNode &node) 
  { 
    node["start"] >> u;
    node["finish"] >> v;
    return true; 
  }
  virtual bool writeSelf(cv::FileStorage& fs) 
  { 
    fs << "start" << u;
    fs << "finish" << v;
    return true; 
  };
};

class AFO_Rect : public AFrameObject  // прямоугольник "общего назначения"
{
public:
  AFO_Rect( std::string type, cv::Rect rect, int flags ):
    AFrameObject( type, rect, flags )
    {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(255,255,0); } 
  virtual int getDrawThickness() { return 2; } 
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::rectangle( display, rect, color, thickness );
  }
};

class AFO_Triangle : public AFO_Polygon  // треугольник "общего назначения"
{
public:
  AFO_Triangle( std::string type, const std::vector<cv::Point>& pts, int flags ):
    AFO_Polygon( type, pts, flags )
    {}
};

class AFO_Quad : public AFO_Polygon  // четурехугольник без самопересечений "общего назначения"
{
public:
  AFO_Quad( std::string type, const std::vector<cv::Point>& pts, int flags ):
    AFO_Polygon( type, pts, flags )
    {}
  AFO_Quad( std::string type, cv::Rect rect, int flags ):
    AFO_Polygon( type, pts, flags )
    {}
};

//class AFO_Line : public AFrameObject  // прямая "общего назначения". Note:охватывающим прямоугольником является прямоугольник кадра. При масштабировании кадра меняются коэффициенты линии
//{
//  cv::Point3d abc;
//public:
//  AFO_Line( std::string type, const std::vector<cv::Point>& pts, int flags ):
//    AFrameObject( type, pts, flags )
//    {}
//  AFO_Quad( std::string type, cv::Rect rect, int flags ):
//    AFrameObject( type, pts, flags )
//    {}
//};

////////////////////////////////////////////////////////////////////////////
class AFO_RedLight // красный огонек, неважно какого светофора (корневой класс) или "стоп-сигнала"
  : public AFrameObject  // --- вообще то он из пятна должен быть выведен...
{
public:
  AFO_RedLight( std::string type, cv::Rect rect, int flags ):
    AFrameObject( type, rect, flags )
  {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(200,200,0); } // один цвет рамки на всех потомков
};

class AFO_GreenLight // зеленый огонек, неважно какого светофора (корневой класс) 
  : public AFrameObject
{
public:
  AFO_GreenLight( std::string type, cv::Rect rect, int flags ):
    AFrameObject( type, rect, flags )
  {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(200, 0, 200); } // иногда проще только цвет переопределить
};

class AFO_RedStopLights // коллекция красных огоней стоп-сигналов одного автомобиля
  : public AFO_RedLight
{
public:
  AFO_RedStopLights( std::string type, cv::Rect rect, int flags ):
    AFO_RedLight( type, rect, flags )
  {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(200,200,0); } // один цвет рамки на всех потомков
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::Rect& r = rect;
    cv::rectangle( display, r, color, thickness );
    cv::Point lu( r.x, r.y );
    cv::Point rb( r.x+r.width-1, r.y+r.height-1 ); 
    cv::Point ru( r.x+r.width-1, r.y );
    cv::Point lb( r.x, r.y+r.height-1 );

    cv::line( display, lu, rb, color, thickness );
    cv::line( display, ru, lb, color, thickness );
  }
};

class AFO_RedManSingle : public AFO_RedLight // красный чел без таймера
{
public:
  AFO_RedManSingle( std::string type, cv::Rect rect, int flags ):
    AFO_RedLight( type, rect, flags )
  {}
  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::Rect& r = rect;
    cv::rectangle( display, r, color, thickness );
    cv::Point lu( r.x, r.y );
    cv::Point rb( r.x+r.width-1, r.y+r.height-1 ); 
    cv::Point ru( r.x+r.width-1, r.y );
    cv::Point lb( r.x, r.y+r.height-1 );

    cv::Point c( r.x+r.width/2, r.y+r.height/2 );
    cv::line( display, (lu+ru)*0.5, c, color, thickness );
    cv::line( display, c, lb, color, thickness );
    cv::line( display, c, rb, color, thickness );
  }
};

class AFO_GreenManSingle : public AFO_GreenLight // зеленый чел без таймера
{
public:
  AFO_GreenManSingle( std::string type, cv::Rect rect, int flags ):
    AFO_GreenLight( type, rect, flags )
  {}
  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::Rect& r = rect;
    cv::rectangle( display, r, color, thickness );
    cv::Point lu( r.x, r.y );
    cv::Point rb( r.x+r.width-1, r.y+r.height-1 ); 
    cv::Point ru( r.x+r.width-1, r.y );
    cv::Point lb( r.x, r.y+r.height-1 );
    cv::Point c( r.x+r.width/2, r.y+r.height/2 );

    cv::line( display, lu, rb, color, thickness );
    cv::line( display, c, (c+lb)*0.5, color, thickness );
    cv::line( display, (c+lb)*0.5, (lb+rb)*0.5, color, thickness );
    cv::line( display, (lu+c)*0.5, (ru+c)*0.5, color, thickness );

    //cv::Point cb = ( r.x+r.width/2, r.y+r.height/2 );
    //cv::line( display, (lu+ru)*0.5, c, color, thickness );
    //cv::line( display, c, lb, color, thickness );
    //cv::line( display, c, rb, color, thickness );

  }
};

class AFO_RedManOverTimer : public AFO_RedLight // красный чел над таймером
{
public:
  AFO_RedManOverTimer( std::string type, cv::Rect rect, int flags ):
    AFO_RedLight( type, rect, flags )
  {}
  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::Rect& r = rect;
    cv::rectangle( display, r, color, thickness );
    cv::Point lu( r.x, r.y );
    cv::Point rb( r.x+r.width-1, r.y+r.height-1 ); 
    cv::Point ru( r.x+r.width-1, r.y );
    cv::Point lb( r.x, r.y+r.height-1 );

    cv::line( display, (lu+lb)*0.5, (ru+rb)*0.5, color, thickness );
    cv::Point c( r.x+r.width/2, r.y+r.height/2 );
    cv::Point cb( r.x+r.width/2, r.y+r.height );
    cv::line( display, c, cb, color, thickness );
  }
};

//
//class AFO_Stoplight_RedCircleSingle : public AFrameObject  // круглый красный сигнал светофора без таймера
//{
//public:
//  AFO_RedManOverTimer( std::string type, cv::Rect rect, int flags ):
//    AFrameObject( type, rect, flags )
//  {}
//  virtual cv::Scalar getDrawColor() { return cv::Scalar(0,255,255); } // иногда проще только цвет переопределить
//  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
//};

class AFO_GreenManBelowTimer : public AFO_GreenLight // зеленый чел под таймером
{
public:
  AFO_GreenManBelowTimer( std::string type, cv::Rect rect, int flags ):
    AFO_GreenLight( type, rect, flags )
  {}
  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
  virtual void draw( cv::Mat& display ) 
  { // умолчательный вариант отрисовки
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::Rect& r = rect;
    cv::rectangle( display, r, color, thickness );
    cv::Point lu( r.x, r.y );
    cv::Point rb( r.x+r.width-1, r.y+r.height-1 ); 
    cv::Point ru( r.x+r.width-1, r.y );
    cv::Point lb( r.x, r.y+r.height-1 );

    cv::line( display, (lu+lb)*0.5, (ru+rb)*0.5, color, thickness );
    cv::Point c( r.x+r.width/2, r.y+r.height/2 );
    cv::Point cu( r.x+r.width/2, r.y );
    cv::line( display, c, cu, color, thickness );
  }
};

class AFO_BlueSquare : public AFrameObject // синий квадрат (знак пешеходного перехода)
{
public:
  AFO_BlueSquare( std::string type, cv::Rect rect, int flags ):
    AFrameObject( type, rect, flags )
  {}
  virtual cv::Scalar getDrawColor() { return cv::Scalar(0,200,200); } // иногда проще только цвет переопределить
  virtual int getDrawThickness() { return 2; } // иногда проще только толщину линий переопределить
  virtual void draw( cv::Mat& display ) 
  { // символически -- знак пешеходного перехода -- треугольник в квадрате
    cv::Scalar color = getDrawColor();
    int thickness = getDrawThickness();
    cv::Rect& r = rect;
    cv::rectangle( display, r, color, thickness );
    cv::Point top( r.x + r.width/2, r.y );
    cv::Point rb( r.x+r.width-1, r.y+r.height-1 ); 
    cv::Point lb( r.x, r.y+r.height-1 );

    cv::line( display, top, rb, color, thickness );
    cv::line( display, top, lb, color, thickness );
  }
};


class FrameData  // данные, распознанные или размеченные на кадре
{
public:
  std::vector< cv::Ptr< AFrameObject > > objects;	
};
////// segm
//////inline AFrameObject* CreateAFrameObject( std::string type, cv::Point u, cv::Point v, int flags=0 ) // factory??
//////{
//////  if (type == "AFO_Point")
//////    return new AFO_Point( type, rect.center(), flags );
//////
//////}

inline AFrameObject* CreateAFrameObject( // создай объект указанного типа из массива точек (уже уточненных)
  std::string type, 
  const std::vector<cv::Point>& pts, 
  int flags=0 ) 
{
  int numpoints = pts.size();
  switch (numpoints)
  {
  case 1:
    if (type == "AFO_Point")
      return new AFO_Point( type, pts[0], flags );
    break;
  case 2:
    if (type == "AFO_Segm")
      return new AFO_Segm( type, pts[0], pts[1], flags );
    {// rect based
      cv::Rect rect(pts[0],pts[1]);
      if (type == "AFO_Rect")
        return new AFO_Rect( type, rect, flags );
      if (type == "AFO_Point")
        return new AFO_Point( type, center(rect), flags );

      ////////////// oh, how to drop following sheet anywhere
      if (type == "AFO_RedManSingle")
        return new AFO_RedManSingle( type, rect, flags );
      if (type == "AFO_RedStopLights")
        return new AFO_RedStopLights( type, rect, flags );
      if (type == "AFO_GreenManSingle")
        return new AFO_GreenManSingle( type, rect, flags );
      if (type == "AFO_RedManOverTimer")
        return new AFO_RedManOverTimer( type, rect, flags );
      if (type == "AFO_GreenManBelowTimer")
        return new AFO_GreenManBelowTimer( type, rect, flags );
      if (type == "AFO_BlueSquare")
        return new AFO_BlueSquare( type, rect, flags );
      return new AFO_Unknown( type, rect, flags );  // объект неопознанного типа
    }
    break;
  case 3:
    if (type == "AFO_Segm"  && pts[0] == pts[2])
      return new AFO_Segm( type, pts[0], pts[1], flags );
    if (type == "AFO_Triangle")
      return new AFO_Triangle( type, pts, flags );
    break;
  case 4:
    if (type == "AFO_Quad")
      return new AFO_Quad( type, pts, flags );

    if (type == "AFO_Triangle" && pts[0] == pts[3])
      return new AFO_Triangle( type, pts, flags );
    break;

  case 5:
    if (type == "AFO_Quad" && pts[0] == pts[4])
    {
      ///pts.pop_back();
      return new AFO_Quad( type, pts, flags );
    }
    break;
  }
  return 0;  // не смогла.. (dummy?)
}


inline AFrameObject* CreateAFrameObject( std::string type, cv::Rect rect=cv::Rect(), int flags=0 ) // factory??
{
  if (type == "AFO_Point")
    return new AFO_Point( type, cv::Point( rect.x, rect.y ), flags );
  if (type == "AFO_Rect")
    return new AFO_Rect( type, rect, flags );
  if (type == "AFO_Segm")
    return new AFO_Segm( type, rect, flags );
  if (type == "AFO_Quad")
    return new AFO_Quad( type, rect, flags );

  if (type == "AFO_RedManSingle")
    return new AFO_RedManSingle( type, rect, flags );
  if (type == "AFO_RedStopLights")
    return new AFO_RedStopLights( type, rect, flags );
  if (type == "AFO_GreenManSingle")
    return new AFO_GreenManSingle( type, rect, flags );
  if (type == "AFO_RedManOverTimer")
    return new AFO_RedManOverTimer( type, rect, flags );
  if (type == "AFO_GreenManBelowTimer")
    return new AFO_GreenManBelowTimer( type, rect, flags );
  if (type == "AFO_BlueSquare")
    return new AFO_BlueSquare( type, rect, flags );
  return new AFO_Unknown( type, rect, flags );  // объект неопознанного типа
}

inline AFrameObject* readFrameObject(cv::FileNode &node)
{
  std::string type("None");
  cv::Rect rect;
  int flags=0; 
  AFrameObject* afo = NULL;
  try {
    node["type"]    >> type;

    //node["x"]       >> rect.x;
    //node["y"]       >> rect.y;
    //node["width"]   >> rect.width;
    //node["height"]  >> rect.height;
    node["rect"]    >> rect;

    node["flags"]   >> flags;
    afo = CreateAFrameObject( type, rect, flags );
  } 
  catch (...) 
  {
    cout << "readFrameObject() failed" << endl;
    return NULL; // unknown object type or no type tag at all
  }

  if (afo)
    afo->readSelf(node); 

  return afo;
}

inline void writeFrameObject( cv::FileStorage& fs, AFrameObject* afo )
{
  fs << "{:";
  fs << "type"    << afo->type;

  //fs << "x"       << afo->rect.x;
  //fs << "y"       << afo->rect.y;
  //fs << "width"   << afo->rect.width;
  //fs << "height"  << afo->rect.height;
  fs << "rect" << afo->rect;

  fs << "flags"   << afo->flags;
  afo->writeSelf(fs);
  fs << "}";
}

inline FrameData readFrameData(cv::FileNode &node)
{
  FrameData fd;
  if (!node.empty())
  {
    cv::FileNode objs = node["FrameObjects"];
    for (cv::FileNodeIterator it = objs.begin(); it != objs.end(); ++it)
    {
      AFrameObject* fo = readFrameObject(*it);
      fd.objects.push_back( cv::Ptr<AFrameObject>(fo) );
    }
  }
  return fd;
}

inline void writeFrameData( cv::FileStorage& fs, FrameData& fd )
{
  fs << "{:";
  fs << "FrameObjects" << "[";
  for (int i=0; i<int(fd.objects.size()); i++)
    writeFrameObject( fs, fd.objects[i] );

  fs << "]";
  fs << "}";
}

inline bool exportFrameData( cv::Mat& base_image, std::string& videofile, int iframe )
{
  string videofolder = cv::format( "%s.dat", videofile.c_str() );
  string framefolder = cv::format( "%s/%05d", videofolder.c_str(), iframe );
  cout << "Frame exported to " << framefolder << endl; 
  return true;
}

inline bool readFrames( 
  cv::FileStorage& fs, 
  vector< FrameData >& frames )
{
  // read frames
  frames.clear();
  cv::FileNode fdaNode = fs["FrameDataArray"];
  if (fdaNode.empty())
    return  false;
  for (cv::FileNodeIterator it = fdaNode.begin(); it != fdaNode.end(); ++it)
  {
    FrameData fd = readFrameData(*it);
    frames.push_back(fd);
  }

  return true;
}


inline bool writeFrames( 
  cv::FileStorage& fs, 
  vector< FrameData >& frames
  )
{
  fs << "FramesCount" << int( frames.size() );
  fs << "FrameDataArray" << "[";
  for (int i=0; i<int(frames.size()); i++)
  {
    writeFrameData( fs, frames[i] );
  }
  fs << "]";
  fs.release();
  return true;
}




#endif // __FRAMEDATA_H