// 2d points clusters DEgenerator
#include "precomp.h"

#include "../../cover_net.h"

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

int find_clusters( // ���������� ����� ��������� (=res_clusters.size())
  vector< Point >&  points, // ���� ����� ����� �� ���������
  //???? int sigma, // ����� ������� ��� ������� �������, �������� �������
  vector< PointsCluster< MyPoint > >& res_clusters, // ���� ��������� �������� { center, points } // { center, sigma }
  int   minPoints = 3,  // ������� ������ ��������� �� ������ ���������� ���-�� �����
  int   maxClusters = 100 // but not more than maxClustersCount
                   )
{
  MetricsL2ReferredToVectorOfPoints2i ruler; // typical ruler
  ruler.samples1 = &points;

  // ��� ���������. ���� �������� ����� ����� -- ����� ��������� ������ �� ���
  // ����� ������������ ������ ��� �����*k^n ��� � -- ����������� ���������� ����, �������� 2

  
  double rootRaius = 20000;
  CoverNet< MyPoint, MetricsL2ReferredToVectorOfPoints2i > cvnet( &ruler, rootRaius, 1 ); // ������ -- points[]

  for (int i=0; i< int( points.size( )); i++)
    cvnet.insert(i);
  cvnet.reportStatistics();
  cvnet.makeClusters( 9, res_clusters );
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

    vector< PointsCluster< MyPoint > > res_clusters; // center+sigma
    find_clusters( points, res_clusters, 10  ); // <<<<<<<<<<<<<<<<<<<<< main pointua

    // draw
    Mat1b img = imread( input_name + ".png", IMREAD_GRAYSCALE );

    if (res_clusters.size() >0 )
    {
      double best_cluster_weight = res_clusters[0].weight; // ������ -- ����� 
      for (int i=0; i< int(res_clusters.size()); i++)
      {
        double this_cluster_weight = res_clusters[i].weight;
        int color = 255 * (1 - this_cluster_weight / best_cluster_weight );
        Point center = points[ res_clusters[i].center ];
        circle( img, center, cvRound( res_clusters[i].radius ), Scalar(color) );
      }
    }

    imshow(input_name, img);
    if (27==waitKey(0))
      break;
  }
}
