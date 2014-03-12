//#include <stdlib.h>
//#include <ctime>

#include "precomp.h"
#include "gen.h"


const double img_height = 512.0;
const double img_width = 1024.0;
const double F = img_width / 2;

class Track
{
public:
  Track(cv::Point3d p1,cv::Point3d p2)
    : m_p1( p1 ), m_p2( p2 ){};

  void make_uniform_motion();//����������
  void print_motion(cv::Mat1b& img, ofstream& out);

  double length();

 
private:
  cv::Point3d m_p1;
  cv::Point3d m_p2;
  std::vector< cv::Point3d > m_points; //����� �� �����
};

//�������� ������������ ��������
void Track::make_uniform_motion()
{
  const double track_length = this->length();
  const int max_points_number = 10;//���������� ����� �� ������ �� ��������� ����� ��������
  //srand( time( 0 ) );
  const int segm_number = (rand() % max_points_number) + 10;// [10..19]

  const cv::Point3d step( 
    (m_p1.x - m_p2.x) / segm_number,
    (m_p1.y - m_p2.y) / segm_number,
    (m_p1.z - m_p2.z) / segm_number );
  
  for(size_t i = 0; i < segm_number + 1; i++)//����� �� ������ segm_number + 1
  {
    cv::Point3d p;
    p.x = m_p2.x + (i * step.x);
    p.y = m_p2.y + (i * step.y);
    p.z = m_p2.z + (i * step.z);
    m_points.push_back(p);
  }
}

double Track::length()
{
  return sqrt(
    (m_p1.x - m_p2.x)*(m_p1.x - m_p2.x) + 
    (m_p1.y - m_p2.y)*(m_p1.y - m_p2.y) +
    (m_p1.z - m_p2.z)*(m_p1.z - m_p2.z) );
}

static Scalar randomColor( RNG& rng )
{
  int icolor = (unsigned) rng;
  return Scalar( icolor&255, (icolor>>8)&255, (icolor>>16)&255 );
}

void Track::print_motion(cv::Mat1b& img, ofstream& out)
 {
   RNG rng;
   std::vector< cv::Point > img_points(m_points.size());
   for(size_t i = 0; i < m_points.size(); i++)
   {
     img_points[i].x = -1 * (F * m_points[i].x / m_points[i].z) + 256;
     img_points[i].y = F * m_points[i].y / m_points[i].z + 512;

     out << img_points[i].x << " " << img_points[i].y << " "
       << m_points[i].x << " " << m_points[i].y << " " << m_points[i].z << endl;
   }
   cv::Point p1( ( F * m_points[0].y / m_points[0].z ) + 512,
     -1 * ( F * m_points[0].x / m_points[0].z ) + 256);
   
   const int index = img_points.size() - 1;
   cv::Point p2( ( F * m_points[index].y / m_points[index].z ) + 512,
     -1 * ( F * m_points[index].x / m_points[index].z ) + 256);

   cv::line( img, p1, p2, randomColor(rng), 2 );
   //�� ��������� 
   for(size_t i = 0; i < img_points.size(); i++)
   {
     img[ img_points[i].x ][ img_points[i].y ] = 0;
   }
 }


//(x, y, z) ������, z ��������� ��������
//������� ���������� ��������
//���������� �� ��������� ����������� �� �������������� ���������
void building_pyramid( std::vector< cv::Point3d >& pyramid, const double dist )//��������� ������� ��������� �������� 
{
  const double half_height = img_height / 2; // q_1 __ q_2
  const double half_width = img_width / 2;   //  |      |
  for(size_t i = 0; i < 4; i++ )             // q_4 __ q_3
  {
    pyramid[i].x = half_height;
    pyramid[i].y = half_width;
    pyramid[i].z = F;
  }
  for(size_t i = 4; i < 8; i++)
  {
    pyramid[i].x = (F + dist) * (half_height / F);
    pyramid[i].y = (F + dist) * (half_width / F);
    pyramid[i].z = F + dist;
  }

  pyramid[0].y = -1 * pyramid[0].y; 
  pyramid[2].x = -1 * pyramid[2].x;
  pyramid[3].x = -1 * pyramid[3].x;
  pyramid[3].y = -1 * pyramid[3].y;

  pyramid[4].y = -1 * pyramid[4].y;
  pyramid[6].x = -1 * pyramid[6].x;
  pyramid[7].x = -1 * pyramid[7].x;
  pyramid[7].y = -1 * pyramid[7].y;
}

cv::Point3d gen_point_on_interval(const cv::Point3d& p1, const cv::Point3d& p2)
{
  //srand( time( 0 ) );
  const double alpha = (rand() % 101) * 1.0 / 100; // [0..1]
  cv::Point3d p(
    alpha * p1.x + (1 - alpha) * p2.x,
    alpha * p1.y + (1 - alpha) * p2.y,
    alpha * p1.z + (1 - alpha) * p2.z );
  return p;
}

//��������� ����� ������ ��������
cv::Point3d gen_point_inside( const std::vector< std::pair< Point3d, Point3d > >& edges )
{
  //srand( time( 0 ) );
  //��������� ����� �� ����� /2 ��������� ����� -> �� ��� �����
  int edge_number = rand() % edges.size(); //������ ��������� �����
  cv::Point3d point_on_edge1 = gen_point_on_interval( edges[edge_number].first, edges[edge_number].second );

  edge_number = rand() % edges.size();
  cv::Point3d point_on_edge2 = gen_point_on_interval( edges[edge_number].first, edges[edge_number].second  );

  //��������� ����� �� ������, ������ ��������� ��������
  return gen_point_on_interval(point_on_edge1, point_on_edge2);
}

Track gen_track_inside_pyramid( const std::vector<Point3d>& pyr )
{
  //���������� �����
  std::vector< std::pair<Point3d, Point3d> > edges(12);
  for(size_t i = 0; i < 3; i++)
  {
    edges[i] = std::make_pair( pyr[i], pyr[i+1]);
  }
  edges[3] = std::make_pair( pyr[3], pyr[0]);
  for(size_t i = 4; i < 7; i++)
  {
    edges[i] = std::make_pair( pyr[i], pyr[i+1]);
  }
  edges[7] = std::make_pair( pyr[7], pyr[4]);
  for(size_t i = 8; i < 12; i++)
  {
    edges[i] = std::make_pair( pyr[i - 8], pyr[i - 4]);
  }

  //����� ������ ��������
  cv::Point3d p1 = gen_point_inside(edges);
  cv::Point3d p2 = gen_point_inside(edges);
  while(p1 == p2)
  {
    p2 = gen_point_inside(edges);
  }


  return  Track(p1, p2);//������ � ����� ����������
}

void testgen_points3d_line_vconst( string res_folder )
{  
  const int dist = 3 * img_width;//���������� �� ��������� ����������� �� ���������, ��� ��������
  std::vector< cv::Point3d > pyramid(8,0);
  building_pyramid(pyramid, dist);//�������� �� ����� ��������� ����, ������� �������� ��� �������������� �� ��������� �����������
 
  const int max_img_count = 10;
  for(size_t i = 0; i < max_img_count; i++)
  {
    Mat1b res( img_height+1, img_width+1, 255 );
    string test_name = res_folder + format( "line%.03d", i+1 );
    ofstream out((test_name + ".txt").c_str());
    out << i+1 << endl;//���������� ������
    for(size_t j = 0; j < i+1; j++)
    {
      Track tr = gen_track_inside_pyramid(pyramid);//��������� ������ ������ �������� ��� ��������
      tr.make_uniform_motion();//������� �������� �����������(����� ����������� ������������ ���������) 
      tr.print_motion(res, out);
    }
    imwrite( test_name+".png", res );
  }
}
