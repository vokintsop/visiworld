// 2d points clusters DEgenerator
#include "precomp.h"

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
#include "../../cover_net.h"
void find_clusters( vector< Point >&  points, // есть набор точек на плоскости
                    //???? int sigma, // будем считать что разброс изестен, кластеры круглые
                    vector< pair< Point, int > >& res_clusters // надо построить кластеры { center, sigma }
                   )
{
  MetricsL2ReferredToVectorOfPoints2i ruler; // tipical ruler
  ruler.samples1 = &points;
  CoverNet< int, MetricsL2ReferredToVectorOfPoints2i > cvnet( &ruler, 20000, 1 ); // индекс -- points[]

  for (int i=0; i<points.size(); i++)
    cvnet.insert(i);

  cvnet.reportStatistics();
  cout << "find_clusters()" << endl; // todo
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

void  testrun_points2points_2d( const string& input_template,  const string& output_template ) // фолдеры могут совпадать
{
  int num_tests=8;
  for (int test=1; test<num_tests; test++)
  {
    string input_name = format( input_template.c_str(), test ); // без расширения
    ifstream ifs( (input_name + ".txt").c_str() );
    // read clusters
    int num_clusters=0;
    vector< pair< Point, int > > clusters;
    ifs >> num_clusters;
    for (int icl=0; icl<num_clusters; icl++)
    {
      int x=0,y=0,sigma=0;
      ifs >> x >> y >> sigma;
      clusters.push_back( make_pair( Point( x, y ), sigma ) );
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

    vector< pair< Point, int > > res_clusters; // center+sigma
    find_clusters( points, res_clusters ); // <<<<<<<<<<<<<<<<<<<<< main point

    Mat1b img = imread( input_name + ".png", IMREAD_GRAYSCALE );

    imshow(input_name, img);
    if (27==waitKey(0))
      break;
  }
}
