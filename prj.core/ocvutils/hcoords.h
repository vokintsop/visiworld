
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

inline double length( const cv::Point3d& v ){  return sqrt(v.ddot(v)); }

inline cv::Point3d& normalize( cv::Point3d& v ) // set length == 1
{
  double len = length(v);
  v = (1./len)* v;
  return v;
}

typedef cv::Point3d HPoint3d; // точка на единичной сфере (as double)
typedef cv::Point3d HLine3d; // линия, как ее вектор нормали на единичной сфере (as double)

struct HCoords
{
  int width, height, depth; // image bitmap size and camera angle
  HCoords(
  int width, // tmp
  int height, // tmp
  int depth = 0 // tmp
  ): width(width), height(height), depth(depth)
  {
    if (depth == 0)
      depth = width/2; // предположим, что 90 градусов по горизонтали
  }

  cv::Point3d convert( // << результат = нормализованный вектор
  const cv::Point& p // на входе точка в исходных координатах изображения
                )
  {
    if (depth == 0)
      depth = width/2; // 90 градусов по умолчанию
    cv::Point3d res = cv::Point3d( p.x - width/2, p.y - height/2, depth );
    return normalize( res );
  }

  void hline2points( 
    const cv::Point3d& hline, // линия в однородных координатах
    cv::Point& pt1, cv::Point& pt2 // линия как две точки на краях битмапа
    )
  {
    // hline == A, B, C,  Ax+By+Cz = 0; z = depth;
    double A = hline.x;
    double B = hline.y;
    double CC = -hline.z*depth;
    cv::Point2d h1, h2;
    if (abs(A) > abs(B))
    {
      h1.y = -height/2.; h1.x = (CC - B*h1.y) / A;
      h2.y =  height/2.; h2.x = (CC - B*h2.y) / A;
    }
    else
    {
      h1.x = -width/2.; h1.y = (CC - A*h1.x) / B;
      h2.x =  width/2.; h2.y = (CC - A*h2.x) / B;
    }
    pt1.x = (int)(h1.x + width/2); 
    pt2.x = (int)(h2.x + width/2); 
    pt1.y = (int)(h1.y + height/2); 
    pt2.y = (int)(h2.y + height/2); 
  }

  HPoint3d point2hpoint( cv::Point& pt ) // проекция точки на сферу радиуса 1. //// <radius>
  { // <x,y,depth> --> sphere
    pt.x -= width/2;
    pt.y -= height/2;
    double denom = sqrt( double( pt.x*pt.x + pt.y*pt.y + depth*depth ) ); //// / radius == 1.;
    return cv::Point3d( pt.x / denom, pt.y / denom, depth / denom );
  }

  bool segment2hline( cv::Point& pt1, cv::Point& pt2, HLine3d& res_hline, double delta = 0.000001 ) // отрезок на картинке --> однородная линия
  {
    HPoint3d hp1 = point2hpoint( pt1 );
    HPoint3d hp2 = point2hpoint( pt2 );
    res_hline = hp1.cross( hp2 );
    double len = length(res_hline);
    if ( len < delta )
      return false; // ответ -- вранье, слишком близкие точки
    res_hline = (1./len) * res_hline;
    return true;
  }
};
