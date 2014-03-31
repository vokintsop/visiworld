
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
};
