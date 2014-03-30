// 2d points clusters generator
#include "precomp.h"
#include "gen.h"


int xMax = 1024;
int yMax = 512;


static int sigma = 20; // радиус кластера, сигма
static int countOfPointsPerCluster = 300; // количество точек в кластере
static int countOfClusters = 10; // количество кластеров

void  testgen_points2points_2d_setup( int _sigma, int _countOfPointsPerCluster, int _countOfClusters )
{
  sigma = _sigma;
  countOfPointsPerCluster = _countOfPointsPerCluster;
  countOfClusters = _countOfClusters;  
}

void  testgen_points2points_2d( string res_folder )
{
  for ( double num_clusters_f = 1; num_clusters_f < countOfClusters; num_clusters_f *= 1.2 )
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
    vector<Point> p;// все точки в кластере
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      for (int i = 0; i < countOfPointsPerCluster; ++i)
      {
        p.push_back(gen_point(clusters[i_cluster].first, clusters[i_cluster].second));
      }
    }
    random_shuffle(p.begin(), p.end());

    string test_name = res_folder + format( "clu%.03d", num_clusters );
    Mat1b res( yMax+1, xMax+1, 255 );
    ofstream out((test_name + ".txt").c_str());

    out << num_clusters << endl;
    for (int i=0; i<(int)clusters.size(); i++)
    {
      circle( res, clusters[i].first, clusters[i].second, Scalar(128,0,0,0), 2 );
      out <<  clusters[i].first.x << "\t" <<  clusters[i].first.y  << "\t" <<  clusters[i].second << endl;
    }
    out << p.size() << endl; // количество точек
    for (int i = 0; i < (int)p.size(); ++i)
    {
      out << p[i].x << " " << p[i].y << endl;
      //circle( res, p[i], 2, Scalar(0, 0, 0), 2); //сами точки
      if ( p[i].y < 0 ||  p[i].x < 0 )
        continue;
      if ( p[i].y >= yMax ||  p[i].x >= xMax )
        continue;
      res[p[i].y][p[i].x] = 0;
    }
    imwrite( test_name+".png", res );
  }

}
