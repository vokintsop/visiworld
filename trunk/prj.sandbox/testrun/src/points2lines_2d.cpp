// 2d points along Lines clusters generator
#include "precomp.h"



typedef Point3d Line; // ax + by+ c     with double a b c 

//make_points( Point3i&  Point& p1, Point& p2 );

/*
class MetricsL2ReferredToVectorOfPoints2i  //template it 
{
public:
  long long counter; // dbg/stat -- счетчик вызовов

  vector< Point2i > *samples1; // main dataset of points
  MetricsL2ReferredToVectorOfPoints2i() : samples1(0),counter(0) {};

  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    double dst=0;
    Point p1 = (*samples1)[i1];
    Point p2 = (*samples1)[i2];
    dst = sqrt( double( sq(p2.x-p1.x) + sq(p2.y-p1.y) ) );
    counter++;
    return dst;
  }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
typedef int MyPoint;
struct _Cluster
{
  Point center;
  double radius;
  double weight;
  _Cluster():radius(0),weight(0){};
  _Cluster(  Point center, double radius, double weight ):
    center(center), radius(radius),weight(weight)
    {};

};




int find_clusters( // возвращает число кластеров (=res_clusters.size())
  vector< Point >&  points, // есть набор точек на плоскости
  //???? int sigma, // будем считать что разброс изестен, кластеры круглые
  vector< _Cluster >& res_clusters, // надо построить кластеры { center, points } // { center, sigma }
  int   minPoints = 3,  // кластер должен содержать не меньше указанного кол-ва точек
  int   maxClusters = 100 // but not more than maxClustersCount
                   )
{
  MetricsL2ReferredToVectorOfPoints2i ruler; // tipical ruler
  ruler.samples1 = &points;

  // тут интересно. если примерно знаем сигму -- можем настроить дерево на нее
  // задав максимальный радиус как сигма*k^n где к -- коэффициент увеличения сфер, например 2

  
  double rootRaius = 20000;
  CoverNet< MyPoint, MetricsL2ReferredToVectorOfPoints2i > cvnet( &ruler, rootRaius, 1 ); // индекс -- points[]

  for (int i=0; i<points.size(); i++)
    cvnet.insert(i);

  vector< pair< int , int > > proper_spheres; ///<points, index>
  // select proper level of tree
  int i_level =9; // magic, got from tests ~  log(rootRaius) // <<<<<<<<<<<<<<<<<<< ???????
  for (int i=0; i< cvnet.getSpheresCount(); i++)
  {
    const CoverSphere< MyPoint  > & s = cvnet.getSphere( i );
    if (s.level != i_level) 
      continue;
    proper_spheres.push_back( make_pair( s.points, i ) );
  }

  sort( proper_spheres.rbegin(), proper_spheres.rend() );
  
  // upload to res_clusters
  res_clusters.clear();
  for (int i=0; i< proper_spheres.size(); i++)
  {
    if (proper_spheres[i].first < minPoints)
      break; // мало точек в кластере
    if (res_clusters.size() >= maxClusters)
      break; // надоело, слишком много кластеров
    int isphere = proper_spheres[i].second; // отсортировали по качеству, а тут номер сферы
    const CoverSphere< MyPoint  > & s = cvnet.getSphere( isphere );
    MyPoint center = s.center;
    Point xycenter = points[center];
    int rad = cvnet.getRadius( s.level );
    _Cluster cluster( xycenter, rad, s.points );
    res_clusters.push_back( cluster );
  }


  cvnet.reportStatistics();
  cout << "find_clusters()" << res_clusters.size() << endl; // todo

  return res_clusters.size();
}

*/
///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../../cover_net.h"

double length( const Point3d& v )
{
  return sqrt(v.ddot(v));
}

Point3d& normalize( Point3d& v ) // set length == 1
{
  double len = length(v);
  v = (1./len)* v;
  return v;
}

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

void draw_line( 
   Mat1b& img,  
   const Point3d& p, 
   Scalar c ) 
{
  // todo
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
    double dst = acos( dd );
    return dst;
  }
};


int find_lines( // возвращает число линий (=found_lines.size())
  vector< Point >&  points, // есть набор точек на плоскости
  vector< Line >& found_lines, // строим по ним набор прямых
  int   minPoints = 3,  // кластер должен содержать не меньше указанного кол-ва точек в сфере
  int   maxLines = 100 //не надо генерить больше указанного количества линий maxLines
  )
{
  // отметим что углы между векторами на единичной сфере удовлетворяют аксиомам метрики.
  int width = 1024; // tmp
  int height = 512; // tmp


  vector< pair< double, Point3d > > pairwise_crosses; // < cross_len, cross(v_i,v_j) >
  // создаем попарные нормали
  for (int i=0; i < int( points.size() ); i++)
  {
    Point3d pi = n_convert( points[i], width, height );
    for (int j=0; j < i; j++)
    {
      Point3d pj = n_convert( points[j], width, height );
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

  sort( pairwise_crosses.rbegin(), pairwise_crosses.rend());

  // закачиваем в дерево
  MetricsAngleReferredToVectorOfPoints3d ruler; 
  ruler.z_notnegative_vectors = &pairwise_crosses;
  double rootRaius = CV_PI; // максимальный угол между векторами
  double minrad = CV_PI / 1000; // 0.18 градуса скажем -- пусть слипаются
  CoverNet< MyPoint, MetricsAngleReferredToVectorOfPoints3d > cvnet( &ruler, rootRaius, minrad ); // индекс -- points[]

  for (int i=0; i< int(pairwise_crosses.size()); i++)
    cvnet.insert(i);

  cvnet.reportStatistics();

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
    vector< pair< Line, double > > lines_ideal; /// мохнатая линия (a b c) c разбросом точек в сигму 
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
    int num_points=0;
    ifs >> num_points;
    for (int ipnt=0; ipnt<num_points; ipnt++)
    {
      int x=0,y=0;
      ifs >> x >> y;
      points.push_back( Point( x, y ) );
    }

    // so, start test
    vector< Point3d > found_lines;
    find_lines( points, found_lines ); // в ответе линии, отсортированные по качеству

    // draw
    Mat1b img = imread( input_name + ".png", IMREAD_GRAYSCALE );

    if (found_lines.size() >0 )
    {
      double best_line_weight = 1; //found_lines[0].weight; // чернее -- лучше 
      for (int i=0; i< int(found_lines.size()); i++)
      {
        double this_line_weight = 1; //found_lines[i].weight;
        int color = cvRound( 255 * (1. - this_line_weight / best_line_weight ) );

        draw_line( img,  found_lines[i], Scalar(color) );
      }
    }

    imshow(input_name, img);
    if (27==waitKey(0))
      break;
  }
}
