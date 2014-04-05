// 2d lines with vanish points clusters generator
#include "precomp.h"
#include "gen.h"
#include "ocvutils/hcoords.h"


const int countLines = 10;
double sigma = 0.01;

HCoords c(1024, 512);

void draw_Line(Mat &img, Point3d l, Scalar color)
{
  double xMin = -c.width / 2;
  double xMax = c.width / 2;
  double yMin = -c.height / 2;
  double yMax = c.height/ 2;
  Point zero((int)xMax, (int)yMax);
  l.z *= c.depth;
  //cout << l.x << " " << l.y << " " << l.z << endl;
   if (abs(l.x) > abs(l.y))
     line(img, Point((int)((-yMin * l.y - l.z) / l.x), (int)yMin) + zero, Point((int)((-yMax * 1ll * l.y - l.z) / l.x),(int) yMax) + zero, color, 2 );
   else
     line(img, Point((int)xMin, (int)((-xMin * l.x - l.z) / l.y)) + zero, Point((int)xMax, (int)((-xMax * 1ll * l.x - l.z) / l.y)) + zero, color, 2 );
}

void testgen_lines2points_2d( string res_folder )
{
  //HCoords c(1024, 512);
  //cout << c.width << " " << c.height << " " << c.depth << endl;
  c.depth = c.width/2;
  for ( double num_clusters_f = 1; num_clusters_f < 10; num_clusters_f *= 1.2 )
  {
    Mat3b res(c.height, c.width, Vec3b(255, 255, 255) );
    // generate clusters  
    int num_clusters = int(num_clusters_f);
    vector< pair< Point3d, Point3d > > clusters; //кластер = 2 прямые 
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      Point3d a1(rand() % c.width, rand() % c.height, c.depth);
      Point3d a2(rand() % c.width, rand() % c.height, c.depth);
      while (a1 == a2)
      {
        a2 = Point3d(rand() % c.width, rand() % c.height, c.depth);
      }
      Point3d b1(rand() % c.width, rand() % c.height, c.depth);
      Point3d b2(rand() % c.width, rand() % c.height, c.depth);
      while (b1 == b2)
      {
        b2 = Point3d(rand() % c.width, rand() % c.height, c.depth);
      }

      a1.x -= c.width / 2;
      a1.y -= c.height / 2;
      a2.x -= c.width / 2;
      a2.y -= c.height / 2;
      b1.x -= c.width / 2;
      b1.y -= c.height / 2;
      b2.x -= c.width / 2;
      b2.y -= c.height / 2;
      //cout << a1.x << " " << a1.y << " " << a1.z << endl;
      //cout << a2.x << " " << a2.y << " " << a2.z << endl;

      Point3d l1 = a1.cross(a2);
      Point3d l2 = b1.cross(b2);
      l1 = normalize(l1);
      l2 = normalize(l2);
      //cout << l1.x << " " << l1.y << " " << l1.z << endl;

      pair<Point3d, Point3d> center = make_pair(l1, l2);
      clusters.push_back(center);
        
      draw_Line(res, l1, 0);
      draw_Line(res, l2, 0);
    }

    ////////// generate points around clusters
    vector<Point3d> p;// все прямые в кластере
    for (int i_cluster = 0; i_cluster < num_clusters; i_cluster++)
    {
      Scalar color = Scalar(rand() % 256, rand() % 256, rand() % 256) ;
      for (int i1 = 0; i1 < countLines; ++i1)
      {
        double k = 1.0 * (rand() % 1000) / 1000.0 ;
        Point3d c = clusters[i_cluster].first * k + clusters[i_cluster].second * (1.0 - k);
        p.push_back(gen_point_on_sphere(c, sigma));// немного сдвинутая прямая
        //p.push_back(c);
        //cout << c.x << " " << c.y << " " << c.z << endl;
        //cout << p.back().x << " " << p.back().y << " " << p.back().z << endl;
        //cout << endl;
        draw_Line(res, p.back(), color);
      }
    }
    random_shuffle(p.begin(), p.end());

    string test_name = res_folder + format( "line%.03d", num_clusters );
  //  cerr << res_folder << endl;
    ofstream out((test_name + ".txt").c_str());
    out << c.width << " " << c.height << endl;
    out << num_clusters << endl;

    out.precision(6);
    for (int i=0; i<clusters.size(); i++)
    {
      //draw_Line(res, clusters[i].first);
      //draw_Line(res, clusters[i].second);
      out << fixed <<  clusters[i].first.x << "\t" <<  clusters[i].first.y << "\t" << clusters[i].first.z  << "\t" <<  clusters[i].second.x << "\t" << clusters[i].second.y << "\t" << clusters[i].second.z << "\t" << sigma << endl;
    }

    out << p.size() << endl;
    for (int i = 0; i < p.size(); ++i)
    {

      out << fixed << p[i].x << "\t" << p[i].y << "\t" << p[i].z << endl;
    }
    
    imwrite( test_name+".png", res );
   }  
}
