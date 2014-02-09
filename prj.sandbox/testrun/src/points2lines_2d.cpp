// 2d points along Lines clusters generator
#include "precomp.h"

#include "../../cover_net.h"


double length( const Point3d& v ){  return sqrt(v.ddot(v)); }

Point3d& normalize( Point3d& v ) // set length == 1
{
  double len = length(v);
  v = (1./len)* v;
  return v;
}

struct HCoords
{
  int width, height, depth; // image bitmap size and camera angle
  HCoords(
  int width, // tmp
  int height, // tmp
  int depth = 0 // tmp
  ): width(width), height(height), depth(depth)
  {
    if (depth == 0)
      depth = width/2; // предположим, что 90 градусов по горизонтали
  }

  Point3d convert( // << результат = нормализованный вектор
  const Point& p // на входе точка в исходных координатах изображения
                )
  {
    if (depth == 0)
      depth = width/2; // 90 градусов по умолчанию
    Point3d res = Point3d( p.x - width/2, p.y - height/2, depth );
    return normalize( res );
  }

  void hline2points( 
    const Point3d& hline, // линия в однородных координатах
    Point& pt1, Point& pt2 // линия как две точки на краях битмапа
    )
  {
    // hline == A, B, C,  Ax+By+Cz = 0; z = depth;
    double A = hline.x;
    double B = hline.y;
    double CC = -hline.z*depth;
    Point2d h1, h2;
    if (abs(A) > abs(B))
    {
      h1.y = -height/2.; h1.x = (CC - B*h1.y) / A;
      h2.y =  height/2.; h2.x = (CC - B*h2.y) / A;
    }
    else
    {
      h1.x = -width/2.; h1.y = (CC - A*h1.x) / B;
      h2.x =  width/2.; h2.y = (CC - A*h2.x) / B;
    }
    pt1.x = h1.x + width/2; 
    pt2.x = h2.x + width/2; 
    pt1.y = h1.y + height/2; 
    pt2.y = h2.y + height/2; 
  }
};

struct FoundLine
{
  Point pt1, pt2; // в координатах битмапа
  //Point3d Line; // ax + by+ c     with double a b c 
  double weight;
  FoundLine( double weight=0 ): weight(weight){}
};


double n_angle( const Point3d& v, const Point3d& u ) // угол между нормированными на 1 векторами в радианах
{
    // todo
  double res = 0;
  return res;
}

Point3d n_convert( // << результат = нормализованный вектор
  const Point& p, // на входе точка в исходных координатах изображения
  int width, int height, //размеры картинки
  int depth = 0
                )
{
  if (depth == 0)
    depth = width/2; // 90 градусов по умолчанию

  Point3d res = Point3d( p.x - width/2, p.y - height/2, depth );
  return normalize( res );
}

//void draw_line( 
//   Mat1b& img,  
//   const Point3d& p, 
//   Scalar c ) 
//{
//  // todo
//}

template< class Num >
Num force_range( Num mi, Num x, Num ma )
{
  if (x < mi)
    return mi;
  if (x > ma)
    return ma;
  return x;
}

typedef int MyPoint;

class MetricsAngleReferredToVectorOfPoints3d
{
public:
  vector< pair< double, Point3d > >*  z_notnegative_vectors; // вектора для кластеризации по углу, отсортированные по длине
  long long counter; // dbg/stat -- счетчик вызовов
  MetricsAngleReferredToVectorOfPoints3d() : z_notnegative_vectors(0),counter(0) {};

  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    counter++;
    Point3d& p1 = (*z_notnegative_vectors)[i1].second; double len1 = (*z_notnegative_vectors)[i1].first;
    Point3d& p2 = (*z_notnegative_vectors)[i2].second; double len2 = (*z_notnegative_vectors)[i2].first;
    double dd = p1.ddot(p2) / (len1*len2);
    dd = force_range<double>( -1., dd, 1. );
    double dst = acos( dd );
    return dst;
  }
};


int find_lines( // возвращает число линий (=found_lines.size())
  vector< Point >&  points, // есть набор точек на плоскости
  vector< FoundLine >& found_lines, // строим по ним набор прямых
  int   img_width,
  int   img_height,
  int   img_depth=0,
  int   minPoints = 5,  // кластер должен содержать не меньше указанного кол-ва точек в сфере
  int   maxLines = 100 //не надо генерить больше указанного количества линий maxLines
  )
{
  // отметим что углы между векторами на единичной сфере удовлетворяют аксиомам метрики.
  HCoords hcoords(img_width, img_height, img_depth); // конвертер координат из пикселей изображения [0..height-1][0..width-1] в плоскость z == depth

  vector< pair< double, Point3d > > pairwise_crosses; // < cross_len, cross(v_i,v_j) >
  // создаем попарные нормали
  for (int i=0; i < int( points.size() ); i++)
  {
    Point3d pi = hcoords.convert( points[i] );
    for (int j=0; j < i; j++)
    {
      Point3d pj = hcoords.convert( points[j] );
      Point3d xx = pi.cross( pj );
      if (xx.z <0)
        xx *= (-1.); // xx = pj.cross(pi); ??
      double xx_length = length( xx );
      double epsilon = 0.0000001;
      if (xx_length < epsilon)
        continue;
      pairwise_crosses.push_back( make_pair( xx_length, xx ) );
    }
  }

  //sort( pairwise_crosses.rbegin(), pairwise_crosses.rend());
  random_shuffle( pairwise_crosses.begin(), pairwise_crosses.end() );

  // закачиваем в дерево
  MetricsAngleReferredToVectorOfPoints3d ruler; 
  ruler.z_notnegative_vectors = &pairwise_crosses;
  double rootRaius = CV_PI; // максимальный угол между векторами
  double minrad = CV_PI / 1000; // 0.18 градуса скажем -- пусть слипаются
  CoverNet< MyPoint, MetricsAngleReferredToVectorOfPoints3d > cvnet( &ruler, rootRaius, minrad ); // индекс -- points[]

  for (int i=0; i< int(pairwise_crosses.size()); i++)
    cvnet.insert(i);
  cvnet.reportStatistics();

  vector< PointsCluster< MyPoint > > res_clusters;
  cvnet.makeClusters( 8, res_clusters );

  for (int i=0; i< int(res_clusters.size()); i++)
  {
    PointsCluster< MyPoint >& cl = res_clusters[i];
    Point3d& hline = pairwise_crosses[ cl.center ].second;
    FoundLine fl( cl.weight );
    hcoords.hline2points( hline, fl.pt1, fl.pt2 );
    found_lines.push_back( fl );
  }
  return 0;
}

void  testrun_points2lines_2d( const string& input_template,  const string& output_template ) // фолдеры могут совпадать
{
  int num_tests=8;
  for (int test=1; test<num_tests; test++)
  {
    string input_name = format( input_template.c_str(), test ); // без расширения
    ifstream ifs( (input_name + ".txt").c_str() );
    // read ideal lines
    int num_lines=0; // lines
    vector< pair< Point3d, double > > lines_ideal; /// мохнатая линия (a b c) c разбросом точек в сигму 
    ifs >> num_lines;
    for (int icl=0; icl<num_lines; icl++)
    {
      double x=0,y=0,z=0;
      double sigma=0; // in testgen not it is int
      ifs >> x >> y >> z >> sigma;
      lines_ideal.push_back( make_pair( Point3d(x,y,z), sigma ) );
    }
    // read points
    vector< Point > points;
#if 1
    int num_points=0;
    ifs >> num_points;
    for (int ipnt=0; ipnt<num_points; ipnt++)
    {
      int x=0,y=0;
      ifs >> x >> y;
      points.push_back( Point( x, y ) );
    }
#else // test points
    points.push_back( Point(512,  256 ) ); // center
    points.push_back( Point(612,  356 ) ); // line x == y
    points.push_back( Point(712,  456 ) ); // line x == y
#endif

    // draw
    Mat1b img = imread( input_name + ".png", IMREAD_GRAYSCALE );

    // so, start test
    vector< FoundLine > found_lines;
    find_lines( points, found_lines, img.cols, img.rows ); // в ответе линии, отсортированные по качеству


    if (found_lines.size() >0 )
    {
      double best_line_weight = found_lines[0].weight; // чернее -- лучше 
      for (int i=0; i< int(found_lines.size()); i++)
      {
        double this_line_weight = found_lines[i].weight;
        int color = cvRound( 255 * (1. - this_line_weight / best_line_weight ) );
        line( img,  found_lines[i].pt1, found_lines[i].pt2, Scalar(color) );
      }
    }

    imshow(input_name, img);
    if (27==waitKey(0))
      break;
  }
}
