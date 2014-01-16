// 2d points clusters generator
#include "precomp.h"
const int xMax = 1000;
const int yMax = 1000;
const int sigma = 20; // радиус кластера, сигма

void  testgen_points2points_2d( string res_folder )
{
  for ( double num_clusters_f = 1; num_clusters_f < 10; num_clusters_f *= 1.2 )
  {
    // generate clusters
    int num_clusters = int(num_clusters_f);
    vector< pair< Point, int > > clusters;  // кластер = центр + среднеквадратичный радиус
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      Point center( rand() % xMax,  rand() % yMax );
      clusters.push_back( make_pair( center, sigma ) ); // tmp
    }

    ////////// generate points around clusters
    ////////for (double num_points_f =1; num_points_f < 100000; num_points_f *=1.2 )
    ////////{
    ////////  int num_points = int( num_points_f );
    ////////  for (int i=0; i<num_points; i++)
    ////////  {

    ////////  }

    ////////}

    string test_name = res_folder + format( "clu%.03d", num_clusters );
    Mat1b res( 1000, 1000, 255 );
    ofstream out((test_name + ".txt").c_str());
    for (int i=0; i<clusters.size(); i++)
    {
      circle( res, clusters[i].first, clusters[i].second, Scalar(0,0,0,0), 2 );
      out <<  clusters[i].first.x << "\t" <<  clusters[i].first.y  << "\t" <<  clusters[i].second << endl;
    }
    imwrite( test_name+".png", res );
  }

}
