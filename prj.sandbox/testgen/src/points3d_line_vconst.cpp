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

  cv::Point3d make_uniform_motion(const int frame_count);//равномерно
  bool predict_collision(const cv::Point3d& step);//продолжение траектории до плоскости изображения
  void print_motion(cv::Mat1b& img, ofstream& out);

  double length();
 
private:
  cv::Point3d m_p1;
  cv::Point3d m_p2;
  std::vector< cv::Point3d > m_points; //точки на треке
};

//имитация равномерного движения
cv::Point3d Track::make_uniform_motion(const int frame_count)//количество кадров время
{
  const int max_points_number = 10;//количество точек на прямой не превышает этого значения
  //srand( time( 0 ) );
  const int segm_count = (rand() % max_points_number) + frame_count;// [10..19]

  //пусть p1 ближе по оси z, чем p2
  if (m_p1.z > m_p2.z)
  {
    std::swap(m_p1, m_p2);
  }

  //dS = V, т.к t = 1 
  const cv::Point3d step( //единичный вектор направления от p2 к p1
    (m_p1.x - m_p2.x) / segm_count,
    (m_p1.y - m_p2.y) / segm_count,
    (m_p1.z - m_p2.z) / segm_count ); 
  
  for(size_t i = 0; i < frame_count; i++)//точек по количеству кадров
  {
    cv::Point3d p;
    p.x = m_p2.x + (i * step.x);
    p.y = m_p2.y + (i * step.y);
    p.z = m_p2.z + (i * step.z);
    m_points.push_back(p);
  }
  return step;
}

bool Track::predict_collision(const cv::Point3d& step)//расстояние, которое успевает преодолеть объект между 2-я последовательными кадрами
{
  Point3d start = m_points[ m_points.size() - 1 ];//ближайшая к плоскости изображения точка траектории 
  
  //вычисление числа "шагов",оставшихся до плоскости изображения
  const int count_steps = static_cast< int >(abs(F - start.z) / step.z);
  cv::Point3d finish = start + count_steps * step;//точка перед плоскостью изображения,по ней делаем вывод - есть столкновение или нет

  //пересчет в координаты изображения
  cv::Point p;
  p.x = -1 * (F * finish.x / finish.z) + img_height / 2;// 256 = img_height / 2
  p.y = F * finish.y / finish.z + img_width / 2;// 512 = img_width / 2

  if( (0 <= p.x) && (p.x <= img_height) && (0 <= p.y) && (p.y <= img_width) )//если последняя точка предполагаемой траектории проецируется внутрь изображения
  {
    return true;//есть столкновение
  }
  return false;//нет столкновения
}

/*double Track::length()
{
  return sqrt(
    (m_p1.x - m_p2.x)*(m_p1.x - m_p2.x) + 
    (m_p1.y - m_p2.y)*(m_p1.y - m_p2.y) +
    (m_p1.z - m_p2.z)*(m_p1.z - m_p2.z) );
}*/

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
       << m_points[i].x << " " << m_points[i].y << " " << m_points[i].z << " " << i << endl;
   }
   cv::Point p1( ( F * m_p1.y / m_p1.z ) + 512,
     -1 * ( F * m_p1.x / m_p1.z ) + 256);
   
   const int index = img_points.size() - 1;
   cv::Point p2( ( F * m_p2.y / m_p2.z ) + 512,
     -1 * ( F * m_p2.x / m_p2.z ) + 256);
  
   cv::line( img, p1, p2, randomColor(rng), 2 );
   //со смещением 
   for(size_t i = 0; i < img_points.size(); i++)
   {
     img[ img_points[i].x ][ img_points[i].y ] = 0;
   }
 }


//(x, y, z) правая, z протыкает пирамиду
//вершины обрезанной пирамиды
//расстояние от плоскости изображения до ограничивающей пооскости
void building_pyramid( std::vector< cv::Point3d >& pyramid, const double dist )//заполняем вершины урезанной пирамиды 
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

//случайная точка внутри пирамиды
cv::Point3d gen_point_inside( const std::vector< std::pair< Point3d, Point3d > >& edges )
{
  //srand( time( 0 ) );
  //случайная точка на грани /2 случайных ребра -> на них точку
  int edge_number = rand() % edges.size(); //номера случайных ребер
  cv::Point3d point_on_edge1 = gen_point_on_interval( edges[edge_number].first, edges[edge_number].second );

  edge_number = rand() % edges.size();
  cv::Point3d point_on_edge2 = gen_point_on_interval( edges[edge_number].first, edges[edge_number].second  );

  //случайная точка на прямой, внутри урезанной пирамиды
  return gen_point_on_interval(point_on_edge1, point_on_edge2);
}

Track gen_track_inside_pyramid( const std::vector<Point3d>& pyr )
{
  //построение ребер
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

  //точки внутри пирамиды
  cv::Point3d p1 = gen_point_inside(edges);
  cv::Point3d p2 = gen_point_inside(edges);
  while(p1 == p2)
  {
    p2 = gen_point_inside(edges);
  }

  return  Track(p1, p2);//начало и конец траектории
}

//Вывод в файл:
//1с) # прямых 
//2с) номер прямой_{1 = есть столкновение, 0 = нет) 
//3с) координата по высоте_координата по ширине (на изображении)_3D координаты_t 
//повтор 2) и 3) по количеству прямых 
void testgen_points3d_line_vconst( string res_folder )
{  
  const int dist = 3 * img_width;//расстояние от плоскости изображения до плоскости, ограничивающей пирамиду
  std::vector< cv::Point3d > pyramid(8,0);
  building_pyramid(pyramid, dist);//пирамида из точек реального мира, которые окажутся при проектировании на плоскости изображения
 
  const int max_img_count = 10;
  for(size_t i = 0; i < max_img_count; i++)//по количеству рисунков
  {
    Mat1b res( img_height+1, img_width+1, 255 );
    string test_name = res_folder + format( "line%.03d", i+1 );
    ofstream out((test_name + ".txt").c_str());
    out << i+1  << endl;//количество прямых -> в файл
    const int frame_count = rand() % 10 + 5;
    for(size_t j = 0; j < i+1; j++)//по количеству прямых на рисунке
    {
      Track tr = gen_track_inside_pyramid(pyramid);//генерация прямой внутри пирамиды без верхушки
      cv::Point3d step = tr.make_uniform_motion(frame_count);//считаем движение равномерным(линия разбивается равномерными отрезками)
      bool collision_will_happen = tr.predict_collision(step);
      out << j+1 << " " << collision_will_happen << endl ;//какая по счету прямая в файле и случится ли столкновение
      tr.print_motion(res, out);
    }
    imwrite( test_name+".png", res );
  }
}
