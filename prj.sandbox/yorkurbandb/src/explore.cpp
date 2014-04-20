// explore.cpp

#include "ocvutils/precomp.h"
#include "ocvutils/ocvkeys.h"
#include "ocvutils/ocvgui.h"
#include "imagerecord.h"


using namespace std;
using namespace cv;


void ImageRecord::select_cluster_candidates_to_clusters(
    CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
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
    hcoords.hline2points( hlines[i], p1, p2 );
    line( mat, p1, p2, colors[segments2vp_truth[i]] );
  }

  imshow( "hlines", mat );
  waitKey(10);
}

void ImageRecord::show_clusters(
  CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
  std::vector< std::vector< std::pair< int, int > > >& // in: по каждому уровню [<кол-во покрываемых точек, номер сферы>]
      clusters // отсортирован  на каждом уровне по кол-ву покрываемых точек)
      )
{ //////////// show & print hlines

  if (clusters.size() <=0)
    return;
  string imagename = name + ".jpg";
  Mat mat1 = imread( imagename );
  int level = -1;
  for ( ; level+1 < int(clusters.size()) && clusters[level+1].size() > 0; level++ ){};
  if (level == -1)
    return;

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
      Scalar color1( 0,0,255 );// red
      Scalar color2( 128, 200, 128 ); 

      double angle = hline_hpoint_angle(hlines[i], vp);
      angles.push_back(angle);
      if ( angle <= radius  )
        line( mat, p1, p2, color1 ), counter++;
      else
        line( mat, p1, p2, color2 );
    }
    imshow( "clusters", mat );
    setWindowText( "clusters", format("clusters: level=%d #%d intersections=%d lines=%d { %.4f %.4f %.4f } r=%.4f", 
      level, iclust, clusters[level][iclust].first, counter, vp.x, vp.y, vp.z, radius ).c_str()  );

    key = waitKey(0);
    if (key == 27)
      return;
    if (key == kPageUp && level > 0)
    {
      level--;
      iclust = 0;
      continue;
    }
    if ( key == kPageDown && level < int(clusters.size())-1 )
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
    if ( key == kRightArrow && iclust < int(clusters[level].size())-1  )
    {
      iclust++;
      continue;
    }

  } while (1);


}

void ImageRecord::make_vp_couples( 
  CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
  std::vector< std::vector< std::pair< int, int > > >& // in: по каждому уровню [<кол-во покрываемых точек, номер сферы>]
    clusters, // отсортирован  на каждом уровне по кол-ву покрываемых точек
  vector< vector< pair< double, pair< Point3d,  Point3d > > > >& // по каждому уровню [< качество пары, < пара ортогональных точек схода> >]
    couples // отсортирован  на каждом уровне по убывающему качеству пар
      )
{
  couples.resize(clusters.size());
  for (int level = 0; level < int(clusters.size()); level++)
  {
    couples[level].clear();
    double radius = cover_net.getRadius(level);
    for (int iclust=0; iclust< int(clusters[level].size()); iclust++)
    {
      const CoverSphere<int>& isph = cover_net.getSphere( clusters[level][iclust].second );
      Point3d& ivp = hlines_intersections[ isph.center ]; 
      for (int jclust=0; jclust<iclust; jclust++)
      {
        const CoverSphere<int>& jsph = cover_net.getSphere( clusters[level][jclust].second );
        Point3d& jvp = hlines_intersections[ jsph.center ]; 
        
        double angle = hlines_angle( ivp, jvp );
        if ( CV_PI/2. - angle > radius )
          continue;

        // ок, точки схода примерно перпендикулярны
        bool doubtful = false;
        int i_count=0;
        int j_count=0;
        for (int iline = 0; iline < int( hlines.size() ); iline++)
        {
          Point3d& hline = hlines[iline];
          double i_angle = hline_hpoint_angle( hline, ivp );
          double j_angle = hline_hpoint_angle( hline, jvp );
          if ( i_angle > radius && j_angle > radius  )
            continue;
          if ( i_angle <= radius && j_angle <= radius  )
          { // прямая лежит на обеих точках схода... оочень подозрительно. Скорее всего одна из точек схода ложная. Либо это линия горизонта...
            doubtful = true;
            continue;
          }
          if ( i_angle <= radius )
            i_count++;
          if ( j_angle <= radius )
            j_count++;
        }
#if 1
        double quality = i_count + j_count; 
#else
        double quality = sqrt( i_count*i_count + j_count*j_count ) // ... alchemy to investigate
#endif
          couples[level].push_back( make_pair( quality, make_pair( ivp, jvp ) ) );
      } // for jclust
    } // for iclust
    sort( couples[level].rbegin(), couples[level].rend() ); 
  } // for level
}

void ImageRecord::show_couples(//////////// show couples of ortohonal vanish points
  CoverNet< int, UnionSpereAnglesRuler >&  cover_net, // каберне, в котором утоплены пересечения линий
  std::vector< std::vector< std::pair< double, std::pair< cv::Point3d,  cv::Point3d > > > >& // по каждому уровню [< качество пары, < пара ортогональных точек схода> >]
    couples // отсортирован  на каждом уровне по убывающему качеству пар
      )
{ 

  if (couples.size() <=0)
    return;
  string imagename = name + ".jpg";
  Mat mat1 = imread( imagename );
  int level = int( couples.size() )-1;
  for ( ; level >=0; level-- )
  {
    if (couples[level].size() > 0)
      break;
  };
  if (level >= int( couples.size() ))
    return;

  int key = -1;
  int icouple =0;
  if (couples[level].size() >= 0)
  do {
    Mat mat = mat1.clone();
    Scalar color_doubtful( 255,0,255 );// magenta -- line on both vps
    Scalar color1( 0,0,255 );// red -- first vp
    Scalar color2( 0,255,0 );// green -- second vp
    Scalar color_others( 255, 255, 0 ); // others
    int i_counter=0, j_counter=0;
    Point3d& ivp = couples[level][icouple].second.first;
    Point3d& jvp = couples[level][icouple].second.second;
    double radius = cover_net.getRadius(level);

    for (int i=0; i< int(hlines.size()); i++)
    {
      Point p1,p2; 
      hcoords.hline2points( hlines[i], p1, p2 );
      double ivp_angle = hline_hpoint_angle(hlines[i], ivp);
      double jvp_angle = hline_hpoint_angle(hlines[i], jvp);

      if ( ivp_angle <= radius && jvp_angle <= radius  )
      {
        line( mat, p1, p2, color_doubtful ); 
        continue;
      }

      if        ( ivp_angle <= radius  )
        line( mat, p1, p2, color1 ), i_counter++;
      else  if  ( jvp_angle <= radius  )
         line( mat, p1, p2, color2 ), j_counter++;
      else
       line( mat, p1, p2, color_others );
    }
    
    { // нарисуем "horison" -- линию, проходящую через пару точек схода
      Point p1,p2; 
      Point3d hline = jvp.cross(ivp);
      hcoords.hline2points( hline, p1, p2 );
      line( mat, p1, p2, Scalar(0,255,255), 3 );
    }

    imshow( "couples", mat );
    setWindowText( "couples", format("couples: level=%d #%d/%d qua=%f i_count=%d j_count=%d r=%.4f", 
      level, icouple, couples[level].size(),
      couples[level][icouple].first, i_counter, j_counter, radius ).c_str()  );

    key = waitKey(0);
    if (key == 27)
      return;
    if (key == kPageUp && level > 0)
    {
      level--;
      icouple = 0;
      continue;
    }
    if ( key == kPageDown && level+1 < int(couples.size()) )
    {
      level++;
      icouple = 0;
      continue;
    }

    if ( key == kLeftArrow && icouple > 0  )
    {
      icouple--;
      continue;
    }
    if ( key == kRightArrow && icouple+1 < int(couples[level].size())  )
    {
      icouple++;
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

  ////////////////
  //vector< vector< pair< int, int > > > // по каждому уровню [<кол-во покрываемых точек, номер сферы>]
  //  clusters( nlevels ); ///, vector< pair< int, int > > ); << подмножество cluster_candidates[][] с ограничением на близость и приоритетом более сильных
  //select_cluster_candidates_to_clusters( cover_net, cluster_candidates, clusters );

  /////////////////
  //show_clusters( cover_net, cluster_candidates );

  vector< vector< pair< double, pair< Point3d,  Point3d > > > >// по каждому уровню [< качество пары, < пара ортогональных точек схода> >]
    couples( nlevels );
  make_vp_couples( cover_net, cluster_candidates, couples );
  show_couples( cover_net, couples );


  //vector< vector< pair< double, int > > cluster_chains( nlevels );
  //for (int level =0; level< nlevels; level++ )
  //{
  //  for (int i=0; i< cluster_candidates[level].size(); i++)
  //  {

  //  }
  //}
  

  waitKey(0);
  //dbgPressAnyKey();


}