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
    assert( length(p1) < 1.001 );    assert( length(p1) > 0.999 );
    assert( length(p2) < 1.001 );    assert( length(p2) > 0.999 );
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
      Point3d i_j_intersection = normalize( hlines[i].cross( hlines[j] ) );
      i_j_intersection = force_positive_direction( i_j_intersection );
      hlines_intersections.push_back( i_j_intersection );
    }

  UnionSpereAnglesRuler ruler( hlines_intersections );

  //CoverNet< int, UnionSpereAnglesRuler >  cover_net( &ruler, CV_PI, 0.05 ); 
  CoverNet< int, UnionSpereAnglesRuler >  cover_net( &ruler, CV_PI, 0.001 ); 
  for (int i=0; i< int( hlines_intersections.size() ); i++)
  {
    cover_net.insert(i);
  }

  cover_net.reportStatistics();

  // осталось кластеризовать... пусть phi -- максимальное расхождение направлений точек схода
  vector< int > true_weight;
  cover_net.countTrueWeight( true_weight );
  assert( true_weight.size() == cover_net.getSpheresCount() );
  int nlevels = cover_net.getCountOfLevels(); // кол-во уровней в дереве
  vector< vector< pair< int, int > > > // по каждому уровню [<кол-во покрываемых точек, номер сферы>]
    density( nlevels ); ///, vector< pair< int, int > > ); 
  for (int i=0; i< int( true_weight.size() ); i++) // i -- номер сферы
  {
    int level = cover_net.getSphereLevel(i);
    int weight = true_weight[i];
    density[level].push_back( make_pair( weight, i ) );
  }
  for (int i=0; i<cover_net.getCountOfLevels(); i++)
    std::sort( density[i].rbegin(), density[i].rend() ); // на каждом уровне сортируем по числу покрываемых точек

  bool print_next_level = true;
  for (int i=0; i<cover_net.getCountOfLevels(); i++)
  { // из любопытства выведем первые 10 точек -- сколько накрывают
    if (print_next_level)
      printf( "\nlevel= %2.d ", i );
    print_next_level = false;
    for ( int j=0; j < min( 10, int( density[i].size() ) ); j ++)
    {
      printf( "%5.d (%5.d) ", density[i][j].first, density[i][j].second );
      print_next_level = true;
    }
  }

  

  dbgPressAnyKey();



  ////Metr1 ruler;  CoverTree< int, Metr1 > tree( &ruler, 1000, 1 );
  //Metr2 ruler;  CoverTree< int, Metr2 > tree( &ruler, 1000, 1 );

  //for (int i=0; i< int( samples.size() ); i++)
  //  tree.insert( i );

  //tree.reportStatistics( 0, 3 ); 

}