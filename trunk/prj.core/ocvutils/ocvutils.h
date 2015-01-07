#ifndef __OCVUTILS_H
#define __OCVUTILS_H

const int _MinInt_=-0x7fff0000;
const int _MaxInt_= 0x7fff0000;
const cv::Rect _InvertedRect_( _MaxInt_,  _MaxInt_, _MinInt_, _MinInt_ );

inline cv::Rect& operator |= ( cv::Rect& a, const cv::Rect& b )
{
  int le = std::min(a.x, b.x);
  int to = std::min(a.y, b.y);
  int ri = std::max( a.x+a.width, b.x+b.width );
  int bo = std::max( a.y+a.height, b.y+b.height );
  a = cv::Rect( le, to, ri-le, bo-to );
  return a;
}

inline cv::Rect& operator |= ( cv::Rect& a, const int delta ) //расширение/сжатие на delta
{
  a = cv::Rect( a.x-delta, a.y-delta, a.width+delta*2, a.height+delta*2);
  return a;
}


inline cv::Rect bounding( const std::vector< cv::Point >& pts ) // охватывающий прямоугольник массива точек
{
  int minx = _MaxInt_;  int miny = _MaxInt_;
  int maxx = _MinInt_;  int maxy = _MinInt_;
  for (int i=0; i< int(pts.size()); i++)
  {
    minx = std::min( pts[i].x, minx );    miny = std::min( pts[i].y, miny );
    maxx = std::max( pts[i].x, maxx );    maxy = std::max( pts[i].y, maxy );
  }
  return cv::Rect( cv::Point( minx, miny), cv::Point( maxx, maxy ) );
}

inline double l2norm( const cv::Point& a, const cv::Point& b )
{
  cv::Point x = a-b;
  return x.ddot(x);
}

inline cv::Point center( const cv::Rect& rect )
{
  return cv::Point( rect.x + rect.width/2, rect.y + rect.height/2 );
}

inline void operator >> (const cv::FileNode& node, cv::Rect & rect )
{
  std::vector< int > rc;
  node >> rc;
  rect = cv::Rect(rc[0],rc[1],rc[2],rc[3]);
}

inline void operator >> (const cv::FileNode& node, cv::Point & pt )
{
  std::vector< int > p;
  node >> p;
  pt = cv::Point(p[0],p[1]);
}

#endif // __OCVUTILS_H
