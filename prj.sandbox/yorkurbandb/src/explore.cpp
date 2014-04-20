// explore.cpp

#include "ocvutils/precomp.h"
#include "ocvutils/ocvkeys.h"
#include "ocvutils/ocvgui.h"
#include "imagerecord.h"


using namespace std;
using namespace cv;

void ImageRecord::select_cluster_candidates_to_clusters(
    CoverNet< int, UnionSpereAnglesRuler >  cover_net, // каберне, в котором утоплены пересечения линий
    vector< vector< pair< int, int > > >& // in: по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      cluster_candidates, // отсортирован  на каждом уровне по кол-ву покрываемых точек
    vector< vector< pair< int, int > > > // по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      clusters // подмножество cluster_candidates[][] с ограничением на близость и приоритетом более сильных
  )
{
  assert(cluster_candidates.size() == cover_net.getCountOfLevels());
  clusters.clear();
  clusters.resize(cover_net.getCountOfLevels());
  for (int level=0; level<cover_net.getCountOfLevels(); level++)
  { // отберем не конфликтующие  кластеры (не пересекающиеся сферы)
    double diameter = cover_net.getRadius(level) * 2;
    for ( int i=0; i < min( 100, int( cluster_candidates[level].size() ) ); i ++)
    {
      const CoverSphere<int>& sph_i = cover_net.getSphere( cluster_candidates[level][i].second );
      Point3d& vp_i = hlines_intersections[ sph_i.center ];
      bool occupied = false;
      for ( int j=0; j < int( clusters[level].size() ); j ++)
      {
        const CoverSphere<int>& sph_j = cover_net.getSphere( clusters[level][j].second );
        Point3d& vp_j = hlines_intersections[ sph_j.center ];
        if (hlines_angle(vp_i, vp_j) < diameter)
        {
          occupied = true;
          break;
        }
      }
      if (!occupied)
        clusters[level].push_back( cluster_candidates[level][i] );
    }
  }

#if 1 // print
  for (int level=0; level<cover_net.getCountOfLevels(); level++)
  { // из любопытства выведем первые 10 точек -- сколько накрывают
    if (cluster_candidates[level].size() <= 0)
      break;
    printf( "\n\n********level= %2.d ", level );
    printf( "\ncluster candidates: " );
    for ( int j=0; j < min( 10, int( cluster_candidates[level].size() ) ); j ++)
    {
      printf( "\n%02d %5.d (%5.d) ", j, cluster_candidates[level][j].first, cluster_candidates[level][j].second );
      const CoverSphere<int>& sph = cover_net.getSphere( cluster_candidates[level][j].second );
      Point3d& vp = hlines_intersections[ sph.center ];
      printf( "\t%1.5f \t%1.5f \t%1.5f\n", vp.x, vp.y, vp.z );
      for ( int k=0; k < min( 10, int( cluster_candidates[level].size() ) ); k ++)
      {
        const CoverSphere<int>& sph_k = cover_net.getSphere( cluster_candidates[level][k].second );
        Point3d& vp_k = hlines_intersections[ sph_k.center ];  
        double jk_angle = degree( hlines_angle( vp, vp_k ) );
        if (j!=k && (jk_angle < 5 || jk_angle > 85))
          printf( "%02d-%02d %2.1f  ", j, k, jk_angle );
      }
    }
    printf( "\nclusters: " );
    for ( int j=0; j < min( 10, int( clusters[level].size() ) ); j ++)
    {
      printf( "\n02d %5.d (%5.d) ", j, clusters[level][j].first, clusters[level][j].second );
      const CoverSphere<int>& sph = cover_net.getSphere( clusters[level][j].second );
      Point3d& vp = hlines_intersections[ sph.center ];
      printf( "\t%1.5f \t%1.5f \t%1.5f\n", vp.x, vp.y, vp.z );
      for ( int k=0; k < min( 10, int( clusters[level].size() ) ); k ++)
      {
        const CoverSphere<int>& sph_k = cover_net.getSphere( clusters[level][k].second );
        Point3d& vp_k = hlines_intersections[ sph_k.center ];  
        double jk_angle = degree( hlines_angle( vp, vp_k ) );
        if (j!=k && (jk_angle < 5 || jk_angle > 85))
          printf( "%02d-%02d %2.1f  ", j, k, jk_angle );
      }
    }
  }
#endif
}
void ImageRecord::show_segments()
{
    string imagename = name + ".jpg";
    Mat mat = imread( imagename );
    for (int i=0; i< int(segments.size()); i++)
    {
      line( mat, segments[i].first, segments[i].second, colors[segments2vp_truth[i]] );
    }

    imshow( "segments", mat );
    waitKey(10);
}

void ImageRecord::show_hlines()
{ //////////// show & print hlines

  string imagename = name + ".jpg";
  Mat mat = imread( imagename );
  for (int i=0; i< int(hlines.size()); i++)
  {
    Point p1,p2; 
    //cout << "hline: " << hlines[i] << endl;
    //cout << "length: " << length( hlines[i] ) << endl;
    hcoords.hline2points( hlines[i], p1, p2 );
    line( mat, p1, p2, colors[segments2vp_truth[i]] );
  }

  imshow( "hlines", mat );
  waitKey(10);
}

void ImageRecord::show_clusters(
  CoverNet< int, UnionSpereAnglesRuler >  cover_net, // каберне, в котором утоплены пересечения линий
  std::vector< std::vector< std::pair< int, int > > >& // in: по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      clusters // отсортирован  на каждом уровне по кол-ву покрываемых точек)
      )
{ //////////// show & print hlines

  if (clusters.size() <=0)
    return;
  string imagename = name + ".jpg";
  Mat mat1 = imread( imagename );
  int level = 3;///clusters.size()/2;
  int key = -1;
  int iclust =0;
  if (clusters[level].size() >= 0)
  do {
    Mat mat = mat1.clone();

    int counter=0;
    vector< double > angles;
    const CoverSphere<int>& sph = cover_net.getSphere( clusters[level][iclust].second );
    Point3d& vp = hlines_intersections[ sph.center ]; 
    double radius = cover_net.getRadius(level);

    for (int i=0; i< int(hlines.size()); i++)
    {
      Point p1,p2; 
      //cout << "hline: " << hlines[i] << endl;
      //cout << "length: " << length( hlines[i] ) << endl;
      hcoords.hline2points( hlines[i], p1, p2 );
      Scalar color2( 255,0,0 ); // blue
      Scalar color1( 0,0,255 );

      double angle = hline_hpoint_angle( vp, hlines[i] );
      angles.push_back(angle);
      if ( angle <= radius  )
        line( mat, p1, p2, color1 ), counter++;
      else
        line( mat, p1, p2, color2 );
    }
    imshow( "clusters", mat );
    setWindowText( "clusters", format("clusters: level=%d #%d intersections=%d lines=%d { %.4f %.4f %.4f } r=%.4f", 
      level, iclust, clusters[level][iclust].first, counter, vp.x, vp.y, vp.z, radius ).c_str()  );

    key = waitKey(10);
    if (key == 27)
      return;
    if (key == kPageUp && level > 0)
    {
      level--;
      iclust = 0;
      continue;
    }
    if ( key == kPageDown && level < clusters.size()-1 )
    {
      level++;
      iclust = 0;
      continue;
    }

    if ( key == kLeftArrow && iclust > 0  )
    {
      iclust--;
      continue;
    }
    if ( key == kRightArrow && iclust < clusters[level].size()-1  )
    {
      iclust++;
      continue;
    }

  } while (1);


}


void ImageRecord::explore()
{
  show_segments();

  for (int i=0; i< int(segments.size()); i++)
  {
    HLine3d hline;
    if (hcoords.segment2hline( segments[i].first, segments[i].second, hline )) // отрезок не вырожден?
      hlines.push_back( hline );
    else
      assert(0); //убедимся что все отрезки не вырожденные
      //hlines.push_back( HLine3d() );
  }

  show_hlines();

  for (int i=0; i< int( hlines.size() ); i++)
    for (int j=0; j<i; j++ ) // рассмотрим пары 0 <= j < i < hlines.size() 
    {
      Point3d i_j_intersection = normalize( hlines[i].cross( hlines[j] ) );
      //i_j_intersection = force_positive_direction( i_j_intersection );
      hlines_intersections.push_back( i_j_intersection );
    }

  //??? random_shuffle( hlines_intersections.begin(), end() );

  UnionSpereAnglesRuler ruler( hlines_intersections );
  //CoverNet< int, UnionSpereAnglesRuler >  cover_net( &ruler, CV_PI, 0.05 ); 
  CoverNet< int, UnionSpereAnglesRuler >  cover_net( &ruler, CV_PI, CV_PI/180 );
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
    cluster_candidates( nlevels ); ///, vector< pair< int, int > > ); 
  for (int i=0; i< int( true_weight.size() ); i++) // i -- номер сферы
  {
    int level = cover_net.getSphereLevel(i);
    int weight = true_weight[i];
    cluster_candidates[level].push_back( make_pair( weight, i ) );
  }
  for (int i=0; i<cover_net.getCountOfLevels(); i++)
    std::sort( cluster_candidates[i].rbegin(), cluster_candidates[i].rend() ); // на каждом уровне сортируем по числу покрываемых точек

  vector< vector< pair< int, int > > > // по каждому уровню [<кол-во покрываемых точек, номер сферы>]
    clusters( nlevels ); ///, vector< pair< int, int > > ); << подмножество cluster_candidates[][] с ограничением на близость и приоритетом более сильных

  select_cluster_candidates_to_clusters( cover_net, cluster_candidates, clusters );

  show_clusters( cover_net, cluster_candidates );

  //make_vp_triple();


  //vector< vector< pair< double, int > > cluster_chains( nlevels );
  //for (int level =0; level< nlevels; level++ )
  //{
  //  for (int i=0; i< cluster_candidates[level].size(); i++)
  //  {

  //  }
  //}
  

  waitKey(0);
  //dbgPressAnyKey();



  ////Metr1 ruler;  CoverTree< int, Metr1 > tree( &ruler, 1000, 1 );
  //Metr2 ruler;  CoverTree< int, Metr2 > tree( &ruler, 1000, 1 );

  //for (int i=0; i< int( samples.size() ); i++)
  //  tree.insert( i );

  //tree.reportStatistics( 0, 3 ); 

}