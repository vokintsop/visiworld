// 2d points clusters DEgenerator
#include "precomp.h"

class MetricsL2ReferredToVectorOfPoints2i  //template it 
{
public:
  long long counter; // dbg/stat -- ������� �������

  vector< Point2i > *samples1; // main dataset of points
  MetricsL2ReferredToVectorOfPoints2i() : samples1(0),counter(0) {};

  double computeDistance( const int& i1,  const int& i2 )  // ������� � samples[]
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



#include "../../cover_net.h"
int find_clusters( // ���������� ����� ��������� (=res_clusters.size())
  vector< Point >&  points, // ���� ����� ����� �� ���������
  //???? int sigma, // ����� ������� ��� ������� �������, �������� �������
  vector< _Cluster >& res_clusters, // ���� ��������� �������� { center, points } // { center, sigma }
  int   minPoints = 3,  // ������� ������ ��������� �� ������ ���������� ���-�� �����
  int   maxClusters = 100 // but not more than maxClustersCount
                   )
{
  MetricsL2ReferredToVectorOfPoints2i ruler; // tipical ruler
  ruler.samples1 = &points;

  // ��� ���������. ���� �������� ����� ����� -- ����� ��������� ������ �� ���
  // ����� ������������ ������ ��� �����*k^n ��� � -- ����������� ���������� ����, �������� 2

  
  double rootRaius = 20000;
  CoverNet< MyPoint, MetricsL2ReferredToVectorOfPoints2i > cvnet( &ruler, rootRaius, 1 ); // ������ -- points[]

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
      break; // ���� ����� � ��������
    if (res_clusters.size() >= maxClusters)
      break; // �������, ������� ����� ���������
    int isphere = proper_spheres[i].second; // ������������� �� ��������, � ��� ����� �����
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
///////////////////////////////////////////////////////////////////////////////////////////////////////

void  testrun_points2points_2d( const string& input_template,  const string& output_template ) // ������� ����� ���������
{
  int num_tests=8;
  for (int test=1; test<num_tests; test++)
  {
    string input_name = format( input_template.c_str(), test ); // ��� ����������
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

    vector< _Cluster > res_clusters; // center+sigma
    find_clusters( points, res_clusters, 10  ); // <<<<<<<<<<<<<<<<<<<<< main pointua

    // draw
    Mat1b img = imread( input_name + ".png", IMREAD_GRAYSCALE );

    if (res_clusters.size() >0 )
    {
      double best_cluster_weight = res_clusters[0].weight; // ������ -- ����� 
      for (int i=0; i<res_clusters.size(); i++)
      {
        double this_cluster_weight = res_clusters[i].weight;
        int color = 255 * (1 - this_cluster_weight / best_cluster_weight );
        circle( img, res_clusters[i].center, res_clusters[i].radius, Scalar(color) );
      }
    }

    imshow(input_name, img);
    if (27==waitKey(0))
      break;
  }
}
