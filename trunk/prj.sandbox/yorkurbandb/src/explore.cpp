// explore.cpp

#include "ocvutils/precomp.h"
#include "imagerecord.h"


using namespace std;
using namespace cv;

void ImageRecord::select_cluster_candidates_to_clusters(
    CoverNet< int, UnionSpereAnglesRuler >  cover_net, // �������, � ������� �������� ����������� �����
    vector< vector< pair< int, int > > >& // in: �� ������� ������ [<���-�� ����������� �����, ����� �����>]
      cluster_candidates, // ������������  �� ������ ������ �� ���-�� ����������� �����
    vector< vector< pair< int, int > > > // �� ������� ������ [<���-�� ����������� �����, ����� �����>]
      clusters // ������������ cluster_candidates[][] � ������������ �� �������� � ����������� ����� �������
  )
{
  assert(cluster_candidates.size() == cover_net.getCountOfLevels());
  clusters.clear();
  clusters.resize(cover_net.getCountOfLevels());
  for (int level=0; level<cover_net.getCountOfLevels(); level++)
  { // ������� �� �������������  �������� (�� �������������� �����)
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
  { // �� ����������� ������� ������ 10 ����� -- ������� ���������
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


void ImageRecord::explore()
{
  show_segments();

  for (int i=0; i< int(segments.size()); i++)
  {
    HLine3d hline;
    if (hcoords.segment2hline( segments[i].first, segments[i].second, hline )) // ������� �� ��������?
      hlines.push_back( hline );
    else
      assert(0); //�������� ��� ��� ������� �� �����������
      //hlines.push_back( HLine3d() );
  }

  show_hlines();

  for (int i=0; i< int( hlines.size() ); i++)
    for (int j=0; j<i; j++ ) // ���������� ���� 0 <= j < i < hlines.size() 
    {
      Point3d i_j_intersection = normalize( hlines[i].cross( hlines[j] ) );
      i_j_intersection = force_positive_direction( i_j_intersection );
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

  // �������� ��������������... ����� phi -- ������������ ����������� ����������� ����� �����
  vector< int > true_weight;
  cover_net.countTrueWeight( true_weight );
  assert( true_weight.size() == cover_net.getSpheresCount() );
  int nlevels = cover_net.getCountOfLevels(); // ���-�� ������� � ������
  vector< vector< pair< int, int > > > // �� ������� ������ [<���-�� ����������� �����, ����� �����>]
    cluster_candidates( nlevels ); ///, vector< pair< int, int > > ); 
  for (int i=0; i< int( true_weight.size() ); i++) // i -- ����� �����
  {
    int level = cover_net.getSphereLevel(i);
    int weight = true_weight[i];
    cluster_candidates[level].push_back( make_pair( weight, i ) );
  }
  for (int i=0; i<cover_net.getCountOfLevels(); i++)
    std::sort( cluster_candidates[i].rbegin(), cluster_candidates[i].rend() ); // �� ������ ������ ��������� �� ����� ����������� �����

  vector< vector< pair< int, int > > > // �� ������� ������ [<���-�� ����������� �����, ����� �����>]
    clusters( nlevels ); ///, vector< pair< int, int > > ); << ������������ cluster_candidates[][] � ������������ �� �������� � ����������� ����� �������

  select_cluster_candidates_to_clusters( cover_net, cluster_candidates, clusters );


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