#ifndef __IMAGERECORD_H
#define __IMAGERECORD_H

#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ocvutils/hcoords.h"
#include "cover_net/cover_net.h"

typedef std::pair< cv::Point, cv::Point > Segment; // отрезок в координатах изображения
typedef cv::Point3d HLine3d; // линия в однородных координатах

inline double degree( double radian )
{
  return (radian * 180) / CV_PI;
}
inline
double hlines_angle( const cv::Point3d& p1, const cv::Point3d& p2 ) // угол между пересекающимися прямыми, заданными векторами на единичной сфере
{
#if 1
    assert( length(p1) < 1.001 );    assert( length(p1) > 0.999 );
    assert( length(p2) < 1.001 );    assert( length(p2) > 0.999 );
    double cos = p1.ddot( p2 );
    double phi = acos( std::max( -1., std::min( 1., cos ) ) );
    //return phi;
    return std::min( CV_PI-phi, phi ); // не больше пипополам
#else
    return upoints[cp1].ddot( upoints[cp2] );
#endif
}

inline
double hline_hpoint_angle( const cv::Point3d& hline, const cv::Point3d& hpoint ) // отклонение точки от прямой при проекции на единичную сферу
{
#if 1
    assert( length(hline) < 1.001 );    assert( length(hline) > 0.999 );
    assert( length(hpoint) < 1.001 );    assert( length(hpoint) > 0.999 );
    double cos = hpoint.ddot( hline );
    double phi = acos( std::max( -1., std::min( 1., cos ) ) );
    //return phi;
    return std::abs( CV_PI/2 - phi );
#else
    return upoints[cp1].ddot( upoints[cp2] );
#endif
}

class UnionSpereAnglesRuler {
public:
  std::vector< cv::Point3d >& upoints; // ссылка на внешний массив -- мн-во точек на единичной сфере
  UnionSpereAnglesRuler( std::vector< cv::Point3d >& upoints): upoints(upoints){};
  double computeDistance(  int cp1,  int cp2 )
  {
    cv::Point3d& p1 = upoints[cp1];
    cv::Point3d& p2 = upoints[cp2];
    return hlines_angle( p1, p2 );
  }
};

class HX : public cv::Point3d // пересечение двух линий на сфере
{
  double some_data;
};

class ImageRecord // результаты по картинке 
{
public:
  ImageRecord(): hcoords(640, 480), how_to_use(0) 
  {
    colors[0] = cv::Scalar( 100, 0, 200 );
    colors[1] = cv::Scalar( 200, 0, 100 );
    colors[2] = cv::Scalar( 0, 200, 0 );
  };
  // входные данные 
  std::string name; // полное (от корня) имя картинки, без расширения, например, "//testdata/yorkurbandb/P1020171/P1020171"
  HCoords hcoords; // конвертер координат камеры

  // ground truth
  cv::Scalar colors[3]; // цвета, назначенные точкам схода для отрисовки
  cv::Point3d truth[3]; // правильные значения точек схода
  cv::Point3d truth_ort[3]; // ортогонализированные правильные значения точек схода
  double sigma[3];

  // рабочие данные по текущей картинке: 
  std::vector< Segment > segments; // отрезки, выделенные на текущем изображении
  std::vector< int > segments2vp_truth; // номера точек схода, размеченные как ground_truth
  std::vector< HLine3d > hlines; // отрезки, преобразованные к линиям (с учетом параметров камеры)
#if 0
  std::vector< std::vector< HX > > hx; // пересечение [ihline][jhline]=> ?
#else
  std::vector< cv::Point3d > hlines_intersections; // пересечения hlines[] на единичной сфере
#endif
  std::vector< cv::Point3d > vanish_points; // точки схода (с учетом параметров камеры)

  void explore();
  void show_segments();
  void show_hlines();
  void select_cluster_candidates_to_clusters(
    CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
    std::vector< std::vector< std::pair< int, int > > >& // in: по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      cluster_candidates, // отсортирован  на каждом уровне по кол-ву покрываемых точек
      std::vector< std::vector< std::pair< int, int > > > // по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      clusters // подмножество cluster_candidates[][] с ограничением на близость и приоритетом более сильных
  );
  void show_clusters(
  CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
  std::vector< std::vector< std::pair< int, int > > >& // in: по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      clusters // отсортирован  на каждом уровне по кол-ву покрываемых точек)
      );


  void make_vp_couples( 
    CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
    std::vector< std::vector< std::pair< int, int > > >& // in: по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      cluster_candidates, // отсортирован  на каждом уровне по кол-ву покрываемых точек
    std::vector< std::vector< std::pair< double, std::pair< cv::Point3d,  cv::Point3d > > > >& // по каждому уровню [< качество пары, < пара ортогональных точек схода> >]
      couples // отсортирован  на каждом уровне по убывающему качеству пар
        );
  void show_couples(//////////// show couples of ortohonal vanish points
    CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
    std::vector< std::vector< std::pair< double, std::pair< cv::Point3d,  cv::Point3d > > > >& // по каждому уровню [< качество пары, < пара ортогональных точек схода> >]
      couples // отсортирован  на каждом уровне по убывающему качеству пар
        );
  int how_to_use; // 0 - test, 1 - train, 2+ - reserved 

  // на выход
  int results_bee; // результаты обработки картинки для подсчета статистики
};


bool read_image_records( std::string& root, std::vector< ImageRecord >& image_records );
void make_report( std::vector< ImageRecord >& image_records );

#endif // __IMAGERECORD_H