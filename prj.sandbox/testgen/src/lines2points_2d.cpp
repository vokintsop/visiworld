// 2d lines with vanish points clusters generator
#include "precomp.h"

const int w = 1024;
const int h = 512;
const int depth = 512;
const Point2f zero(w / 2, h / 2);

/*Point2f norm(Point3f &p)// из однородных в обычные
{
  Point2f res(p.x / p.z, py / p.z);
  return res;
}
Point3f gen_Line() // генерирует прямую в однородных координатах ? нормировать по длине или z
{
  Point3f res;
  res.x = rand() % 10000 - 20000;
  res.y = rand() % 10000 - 20000;
  res.z = rand() % 10000 - 20000;
  
  res = norm(res);// ???
  return res;
}
Point3f gen_focus(Point3f &L1, Point3f &L2)// Генерирует фокус по двум прямым
{
  Point f = L1.cross(L2);
  return norm(f);
}

void  testgen_lines2points_2d( string res_folder )
{
  for ( double num_clusters_f = 1; num_clusters_f < 10; num_clusters_f *= 1.2 )
  {
    // generate clusters
    int num_clusters = int(num_clusters_f);
    vector< pair< pair<Point3f, Point3f>, int > > clusters; // каждый клвстер - две прямые // кластер = центр + среднеквадратичный радиус
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      pair<Point3f, Point3f> center = make_pair(gen_Line(), gen_Line());
      clusters.push_back( make_pair( center, sigma ) ); // tmp
    }

    ////////// generate points around clusters
    vector<Point3f> p;// все прямые в кластере
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      for (int i = 0; i < countPoints; ++i)
      {
        double k = 1.0 * (rand() % 1000) / 1000.0;
        p.push_back(gen_nearLine(clusters[i].first.first * k + clusters[i].first.second * (1.0 - k), sigma));
      }
    }
    random_shuffle(p.begin(), p.end());

    string test_name = res_folder + format( "line%.03d", num_clusters );
    Mat1b res( yMax+1, xMax+1, 255 );
    ofstream out((test_name + ".txt").c_str());
    out << num_clusters << endl;
    for (int i=0; i<clusters.size(); i++)
    {
      draw_Line(res, clusters[i].first);
      out <<  clusters[i].first.a << "\t" <<  clusters[i].first.b << "\t" << clusters[i].first.c  << "\t" <<  clusters[i].second << endl;
    }
    out << p.size() << endl; // количество точек
    for (int i = 0; i < p.size(); ++i)
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
*/