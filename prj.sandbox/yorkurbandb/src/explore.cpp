// explore.cpp

#include "ocvutils/precomp.h"
#include "imagerecord.h"


using namespace std;
using namespace cv;

#include "cover_net/cover_net.h"

class UnionSpereAnglesRuler {
public:
  vector< Point3d >& upoints; // ссылка на внешний массив -- мн-во точек на единичной сфере
  UnionSpereAnglesRuler( vector< Point3d >& upoints): upoints(upoints){};
  double computeDistance(  int cp1,  int cp2 )
  {
#if 1
    Point3d& p1 = upoints[cp1];
    Point3d& p2 = upoints[cp2];
    double cos = p1.ddot( p2 );
    return acos( max( -1., min( 1., cos ) ) );
#else
    return upoints[cp1].ddot( upoints[cp2] );
#endif
  }
};


void ImageRecord::explore()
{

  {//////////// show segments
    string imagename = name + ".jpg";
    Mat mat = imread( imagename );
    for (int i=0; i< int(segments.size()); i++)
    {
      line( mat, segments[i].first, segments[i].second, Scalar( 100, 0, 200 ) );
    }

    imshow( "segments", mat );
    waitKey(0);
  }

  for (int i=0; i< int(segments.size()); i++)
  {
    HLine3d hline;
    if (hcoords.segment2hline( segments[i].first, segments[i].second, hline )) // отрезок не вырожден?
      hlines.push_back( hline );
  }

  {//////////// show & print hlines
    string imagename = name + ".jpg";
    Mat mat = imread( imagename );
    for (int i=0; i< int(hlines.size()); i++)
    {
      Point p1,p2; 
      //cout << "hline: " << hlines[i] << endl;
      //cout << "length: " << length( hlines[i] ) << endl;
      hcoords.hline2points( hlines[i], p1, p2 );
      line( mat, p1, p2, Scalar( 200, 0, 200 ) );
    }

    imshow( "hlines", mat );
    waitKey(0);
  }

  vector< Point3d > hlines_intersections; 
  for (int i=0; i< int( hlines.size() ); i++)
    for (int j=0; j<i; j++ ) // рассмотрим пары 0 <= j < i < hlines.size() 
    {
      Point3d i_j_intersection = hlines[i].cross( hlines[j] );
      i_j_intersection = force_positive_direction( i_j_intersection );
      hlines_intersections.push_back( i_j_intersection );
    }

  UnionSpereAnglesRuler ruler( hlines_intersections );

  //CoverNet< int, UnionSpereAnglesRuler >  cover_net( &ruler, CV_PI, 0.05 ); 
  CoverNet< int, UnionSpereAnglesRuler >  cover_net( &ruler, CV_PI*16, 0.001 ); 
  for (int i=0; i< hlines_intersections.size(); i++)
  {
    cover_net.insert(i);
  }

  cover_net.reportStatistics();

  dbgPressAnyKey();



  ////Metr1 ruler;  CoverTree< int, Metr1 > tree( &ruler, 1000, 1 );
  //Metr2 ruler;  CoverTree< int, Metr2 > tree( &ruler, 1000, 1 );

  //for (int i=0; i< int( samples.size() ); i++)
  //  tree.insert( i );

  //tree.reportStatistics( 0, 3 ); 

}