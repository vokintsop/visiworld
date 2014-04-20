#ifndef __IMAGERECORD_H
#define __IMAGERECORD_H

#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "ocvutils/hcoords.h"
#include "cover_net/cover_net.h"

typedef std::pair< cv::Point, cv::Point > Segment; // ������� � ����������� �����������
typedef cv::Point3d HLine3d; // ����� � ���������� �����������

inline double degree( double radian )
{
  return (radian * 180) / CV_PI;
}
inline
double hlines_angle( const cv::Point3d& p1, const cv::Point3d& p2 ) // ���� ����� ��������������� �������, ��������� ��������� �� ��������� �����
{
#if 1
    assert( length(p1) < 1.001 );    assert( length(p1) > 0.999 );
    assert( length(p2) < 1.001 );    assert( length(p2) > 0.999 );
    double cos = p1.ddot( p2 );
    double phi = acos( std::max( -1., std::min( 1., cos ) ) );
    //return phi;
    return std::min( CV_PI-phi, phi ); // �� ������ ���������
#else
    return upoints[cp1].ddot( upoints[cp2] );
#endif
}

inline
double hline_hpoint_angle( const cv::Point3d& hline, const cv::Point3d& hpoint ) // ���������� ����� �� ������ ��� �������� �� ��������� �����
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
  std::vector< cv::Point3d >& upoints; // ������ �� ������� ������ -- ��-�� ����� �� ��������� �����
  UnionSpereAnglesRuler( std::vector< cv::Point3d >& upoints): upoints(upoints){};
  double computeDistance(  int cp1,  int cp2 )
  {
    cv::Point3d& p1 = upoints[cp1];
    cv::Point3d& p2 = upoints[cp2];
    return hlines_angle( p1, p2 );
  }
};

class HX : public cv::Point3d // ����������� ���� ����� �� �����
{
  double some_data;
};

class ImageRecord // ���������� �� �������� 
{
public:
  ImageRecord(): hcoords(640, 480), how_to_use(0) 
  {
    colors[0] = cv::Scalar( 100, 0, 200 );
    colors[1] = cv::Scalar( 200, 0, 100 );
    colors[2] = cv::Scalar( 0, 200, 0 );
  };
  // ������� ������ 
  std::string name; // ������ (�� �����) ��� ��������, ��� ����������, ��������, "//testdata/yorkurbandb/P1020171/P1020171"
  HCoords hcoords; // ��������� ��������� ������

  // ground truth
  cv::Scalar colors[3]; // �����, ����������� ������ ����� ��� ���������
  cv::Point3d truth[3]; // ���������� �������� ����� �����
  cv::Point3d truth_ort[3]; // �������������������� ���������� �������� ����� �����
  double sigma[3];

  // ������� ������ �� ������� ��������: 
  std::vector< Segment > segments; // �������, ���������� �� ������� �����������
  std::vector< int > segments2vp_truth; // ������ ����� �����, ����������� ��� ground_truth
  std::vector< HLine3d > hlines; // �������, ��������������� � ������ (� ������ ���������� ������)
#if 0
  std::vector< std::vector< HX > > hx; // ����������� [ihline][jhline]=> ?
#else
  std::vector< cv::Point3d > hlines_intersections; // ����������� hlines[] �� ��������� �����
#endif
  std::vector< cv::Point3d > vanish_points; // ����� ����� (� ������ ���������� ������)

  void explore();
  void show_segments();
  void show_hlines();
  void select_cluster_candidates_to_clusters(
    CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // �������, � ������� �������� ����������� �����
    std::vector< std::vector< std::pair< int, int > > >& // in: �� ������� ������ [<���-�� ����������� �����, ����� �����>]
      cluster_candidates, // ������������  �� ������ ������ �� ���-�� ����������� �����
      std::vector< std::vector< std::pair< int, int > > > // �� ������� ������ [<���-�� ����������� �����, ����� �����>]
      clusters // ������������ cluster_candidates[][] � ������������ �� �������� � ����������� ����� �������
  );
  void show_clusters(
  CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // �������, � ������� �������� ����������� �����
  std::vector< std::vector< std::pair< int, int > > >& // in: �� ������� ������ [<���-�� ����������� �����, ����� �����>]
      clusters // ������������  �� ������ ������ �� ���-�� ����������� �����)
      );


  void make_vp_couples( 
    CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // �������, � ������� �������� ����������� �����
    std::vector< std::vector< std::pair< int, int > > >& // in: �� ������� ������ [<���-�� ����������� �����, ����� �����>]
      cluster_candidates, // ������������  �� ������ ������ �� ���-�� ����������� �����
    std::vector< std::vector< std::pair< double, std::pair< cv::Point3d,  cv::Point3d > > > >& // �� ������� ������ [< �������� ����, < ���� ������������� ����� �����> >]
      couples // ������������  �� ������ ������ �� ���������� �������� ���
        );
  void show_couples(//////////// show couples of ortohonal vanish points
    CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // �������, � ������� �������� ����������� �����
    std::vector< std::vector< std::pair< double, std::pair< cv::Point3d,  cv::Point3d > > > >& // �� ������� ������ [< �������� ����, < ���� ������������� ����� �����> >]
      couples // ������������  �� ������ ������ �� ���������� �������� ���
        );
  int how_to_use; // 0 - test, 1 - train, 2+ - reserved 

  // �� �����
  int results_bee; // ���������� ��������� �������� ��� �������� ����������
};


bool read_image_records( std::string& root, std::vector< ImageRecord >& image_records );
void make_report( std::vector< ImageRecord >& image_records );

#endif // __IMAGERECORD_H