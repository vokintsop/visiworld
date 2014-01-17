// 2d points along lines clusters generator
#include "precomp.h"

const int xMax = 1000;
const int yMax = 1000;
const int sigma = 20; // радиус кластера, сигма
const int countPoints = 500; // количество точек в кластере

void  testgen_points2lines_2d( string res_folder )
{
  void  testgen_points2points_2d( string res_folder )
{
  for ( double num_clusters_f = 1; num_clusters_f < 10; num_clusters_f *= 1.2 )
  {
    // generate clusters
    int num_clusters = int(num_clusters_f);
    vector< pair< Line, int > > clusters;  // кластер = центр + среднеквадратичный радиус
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      Point center( rand() % xMax,  rand() % yMax );
      clusters.push_back( make_pair( center, sigma ) ); // tmp
    }

    ////////// generate points around clusters
    vector<Point> p;// все точки в кластере
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      for (int i = 0; i < countPoints; ++i)
      {
        p.push_back(gen_point(clusters[i_cluster].first, clusters[i_cluster].second));
      }
    }
    random_shuffle(p.begin(), p.end());

    string test_name = res_folder + format( "clu%.03d", num_clusters );
    Mat1b res( 1000, 1000, 255 );
    ofstream out((test_name + ".txt").c_str());
    for (int i=0; i<clusters.size(); i++)
    {
      circle( res, clusters[i].first, clusters[i].second, Scalar(0,0,0,0), 2 );
      out <<  clusters[i].first.x << "\t" <<  clusters[i].first.y  << "\t" <<  clusters[i].second << endl;
    }
    out << p.size() << endl; // количество точек
    for (int i = 0; i < p.size(); ++i)
    {
      out << p[i].x << " " << p[i].y << endl;
      circle( res, p[i], 2, Scalar(0, 0, 0), 2); //сами точки
    }
    imwrite( test_name+".png", res );
  }

}

}
