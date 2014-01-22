// 2d points along Lines clusters generator
#include "precomp.h"
#include "gen.h"

const int xMax = 1000;
const int yMax = 1000;
const int addx = 30;
const int addy = 30;
const int sigma = 10; // радиус кластера, сигма
const int countPoints = 100; // количество точек в кластере


struct Line
{
  double a, b, c;
};
Line from2Points(Point l, Point r)
{
   Line res;
   res.a = l.y - r.y;
   res.b = r.x - l.x;
   res.c = -l.x * res.a - l.y * res.b;
   return res;
}
Line gen_Line() // генерирует случайную прямую
{
   Line res;
   Point l( rand() % xMax,  rand() % yMax );
   Point r( rand() % xMax,  rand() % yMax );
   while (l == r)
   {
     r.x = rand() % xMax;
     r.y = rand() % yMax;
   }
   
   res = from2Points(l, r);
   return res;
}

Point gen_point_on_Line(Line &l) //генерирует случайную точку на прямой 
{
  while (true)
  {
    Point res;
    if (abs(l.a) > abs(l.b))
    {
      res.y = rand() % yMax;
      res.x = ((long long)-res.y * l.b - l.c) / l.a;
    }
    else
    {
      res.x = rand() % xMax;
      res.y = ((long long)-res.x * l.a - l.c) / l.b;
    }
    if (res.x >= 0 && res.x < xMax && res.y >= 0 && res.y < yMax)
      return res;
  }
}

void draw_Line(Mat &img, Line &l)
{
   if (abs(l.a) > abs(l.b))
     line(img, Point((-0ll * l.b - l.c) / l.a, 0), Point((-yMax * 1ll * l.b - l.c) / l.a, yMax), Scalar(128,0,0,0), 2 );
   else
     line(img, Point(0, (-0ll * l.a - l.c) / l.b), Point(xMax, (-xMax * 1ll * l.a - l.c) / l.b), Scalar(128,0,0,0), 2 );
}

void  testgen_points2lines_2d( string res_folder )
{
  for ( double num_clusters_f = 1; num_clusters_f < 10; num_clusters_f *= 1.2 )
  {
    // generate clusters
    int num_clusters = int(num_clusters_f);
    vector< pair< Line, int > > clusters;  // кластер = центр + среднеквадратичный радиус
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      Line center = gen_Line();
      clusters.push_back( make_pair( center, sigma ) ); // tmp
    }

    ////////// generate points around clusters
    vector<Point> p;// все точки в кластере
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      for (int i = 0; i < countPoints; ++i)
      {
        p.push_back(gen_point(gen_point_on_Line(clusters[i_cluster].first), clusters[i_cluster].second));
      }
    }
    random_shuffle(p.begin(), p.end());

    string test_name = res_folder + format( "line%.03d", num_clusters );
    Mat1b res( yMax+1, xMax+1, 255 );
    ofstream out((test_name + ".txt").c_str());
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
