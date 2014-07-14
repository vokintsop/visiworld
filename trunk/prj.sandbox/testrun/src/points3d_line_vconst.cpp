#include "precomp.h"
#include "cover_net/cover_net.h"


const double f = 512;

cv::Point3d cross_product( const cv::Point3d a, const cv::Point3d b )
{
  cv::Point3d c;
  c.x = a.y * b.z - a.z * b.y;
  c.y = a.z * b.x - a.x * b.z;
  c.z = a.x * b.y - a.y * b.x;
  return c;
}

double dot_product( const cv::Point3d a, const cv::Point3d b )
{
  return ( a.x * b.x + a.y * b.y + a.z * b.z );
}


//flying parallelograms
void testrun_points3d_line_vconst( const string& input_folder,  const string& output_folder )
{
  ifstream in( ( input_folder + format( "line%.03d", 8 ) + ".txt" ).c_str() );
  string s1;
  string s2;
  string s3;
  in >> s1 >> s2 >> s3;//количество траекторий, количество кадров(точек на прямой), 
  int track_count = atoi( s1.c_str() );
  int frame_count = atoi( s2.c_str() );
  int l = atoi( s3.c_str() );//расстояние между фарами

  for( size_t i = 0; i < track_count; i++ )//по количеству рисунков
  {
    int track_number = 0;
    bool will_collide = false;//изначально не знаем о столкновении
    in >> track_number >> will_collide;

    cv::Point3d q_0;//в мире
    cv::Point3d m_0;//   /         /
    cv::Point3d q_1;// q_0 ------ m_0
    cv::Point3d m_1;// q_1 ------ m_1 

    cv::Point3d p_0;//в фокальной плоскости
    cv::Point3d r_0;// p_0 ------ r_0
    cv::Point3d p_1;// p_1 ------ r_1
    cv::Point3d r_1;  
    
    
    in >> p_0.x >> p_0.y          //координаты на изображении левой фары
      >> q_0.x >> q_0.y >> q_0.z  //координаты в мире левой фары 
      >> r_0.y                    //координата y правой фары на изображении
      >> m_0.y;                   //координата правой фары в мире, смещенная относительно левой фары на l 
    
    p_0.z = f;
    r_0.x = p_0.x;
    r_0.z = f;
    m_0.x = q_0.x; 
    m_0.z = q_0.z;

    for( size_t j = 0; j < frame_count - 1; j++ )
    {  
      in >> p_1.x >> p_1.y 
        >> q_1.x >> q_1.y >> q_1.z 
        >> r_1.y 
        >> m_1.y;
      
      p_1.z = f;
      r_1.x = p_1.x;
      r_1.z = f;
      m_1.x = q_1.x;
      m_1.z = q_1.z;

      //нормаль n к плоскости параллелограмма
      cv::Point3d p_0_x_r_0 = cross_product( p_0, r_0 );
      cv::Point3d p_1_x_r_1 = cross_product( p_1, r_1 );
      cv::Point3d p_0_x_p_1 = cross_product( p_0, p_1 );
      cv::Point3d r_0_x_r_1 = cross_product( r_0, r_1 );
      cv::Point3d n1 = cross_product( p_0_x_r_0, p_1_x_r_1 );
      cv::Point3d n2 = cross_product( p_0_x_p_1, r_0_x_r_1 );

      cv::Point3d n = cross_product( n1, n2 );

      //найдем константу a из уравнения плоскости (n, x) = a
      double n_p_0 = dot_product(n, p_0);
      double n_r_0 = dot_product(n, r_0);
      cv::Point3d diff( ( p_0.x / n_p_0 ) - ( r_0.x / n_r_0 ),
        ( p_0.y / n_p_0 ) - ( r_0.y / n_r_0 ),
        ( p_0.z / n_p_0 ) - ( r_0.z / n_r_0 ) ); 
      double abs_diff = std::sqrt( diff.x * diff.x + diff.y * diff.y + diff.z * diff.z ); 
      double a = l / abs_diff;
      if( ( a * n_p_0 ) <= 0 )
      {
        a *= -1;
      }

      cv::Point3d res_q_0( ( a * p_0.x ) / n_p_0, ( a * p_0.y ) / n_p_0, ( a * p_0.z ) / n_p_0 );
      cv::Point3d res_m_0( ( a * r_0.x ) / n_r_0, ( a * r_0.y ) / n_r_0, ( a * r_0.z ) / n_r_0 );

      std::cout << q_0 << " " << res_q_0 << std::endl;
      std::cout << m_0 << " " << res_m_0 << "\n" << std::endl;

      p_0 = p_1;
      q_0 = q_1;
      r_0 = r_1;
      m_0 = m_1;
    }
  }
}


#if 0
void testrun_points3d_line_vconst( const string& input_folder,  const string& output_folder )
{
  ifstream in( ( input_folder + format( "line%.03d", 7 ) + ".txt" ).c_str() );
  string s1;
  string s2;
  string s3;
  in >> s1 >> s2 >> s3;//количество траекторий, количество кадров(точек на прямой), 
  int track_count = atoi( s1.c_str() );
  int frame_count = atoi( s2.c_str() );
  int l = atoi( s3.c_str() );//расстояние между фарами

  for( size_t i = 0; i < track_count; i++ )//по количеству рисунков
  {
    int track_number = 0;
    bool will_collide = false;//изначально не знаем о столкновении
    in >> track_number >> will_collide;

    double p_0_x = 0; double p_0_y = 0;
    double p_1_x = 0; double p_1_y = 0;
    double p_2_x = 0; double p_2_y = 0;

    double q_i_x = 0; double q_i_y = 0; double q_i_z = 0;
    double t_1 = 1;  double t_2 = 2;

    in >> p_0_x >> p_0_y >> q_i_x >> q_i_y >> q_i_z; /*>> t_1*///;
    in >> p_1_x >> p_1_y >> q_i_x >> q_i_y >> q_i_z;/*>> t_1*/;
    for( size_t j = 0; j < frame_count - 3 + 1; j++ )
    {

      in >> p_2_x >> p_2_y >> q_i_x >>  q_i_y >> q_i_z /*>> t_2*/;
      double f = 512;

      //std::cout << p_0_x << " " << p_0_y << " " << q_i_x << " " << q_i_y << " " << q_i_z << " " << t << std::endl;

      //Ab = c
      //int rows, int cols, int type
      Mat A = ( Mat_< double >( 8, 8 ) << 
        0   ,   0   ,  1 ,  0 ,  0 ,  0   ,   0  ,   0
        ,   0   ,   0   ,  0 ,  1 ,  0 ,  0   ,   0  ,   0
        , p_1_x ,   0   , -1 ,  0 ,  0 , -t_1 ,   0  ,   0
        , p_1_y ,   0   ,  0 , -1 ,  0 ,   0  , -t_1 ,   0
        ,   0   , p_2_x , -1 ,  0 ,  0 , -t_2 ,   0  ,   0
        ,   0   , p_2_y ,  0 , -1 ,  0 ,   0  , -t_2 ,   0
        ,   0   ,   0   ,  0 ,  0 ,  1 ,   0  ,   0  ,   0
        ,   f   ,   0   ,  0 ,  0 , -1 ,   0  ,   0  , -t_1 );


      /* ,   0   ,   f   ,  0 ,  0 , -1 ,   0  ,   0  , -t_2 );*/
      /*    p_0_x ,   0   ,   0   , -1 ,  0 ,  0 ,   0  ,   0  , 0
      , p_0_y ,   0   ,   0   ,  0 , -1 ,  0 ,   0  ,   0  , 0
      ,   0   , p_1_x ,   0   , -1 ,  0 ,  0 , -t_1 ,   0  , 0
      ,   0   , p_1_y ,   0   ,  0 , -1 ,  0 ,   0  , -t_1 , 0
      ,   0   ,   0   , p_2_x , -1 ,  0 ,  0 , -t_2 ,   0  , 0
      ,   0   ,   0   , p_2_y ,  0 , -1 ,  0 ,   0  , -t_2 , 0
      ,  512  ,   0   ,   0   ,  0 ,  0 , -1 ,   0  ,   0  , 0
      ,   0   ,  512  ,   0   ,  0 ,  0 , -1 ,   0  ,   0  , -t_1
      ,   0   ,   0   ,  512  ,  0 ,  0 , -1 ,   0  ,   0  , -t_2 );*/

      /*Mat A = ( Mat_< double >( 3, 3 ) <<
      p_0_x, p_1_x, p_2_x,
      p_0_y, p_1_y, p_2_y,
      1,        1,    1 );*/

      // Mat C = ( Mat_<  > )


      for(int m = 0; m < A.rows; m++)
      {
        for(int n = 0; n < A.cols; n++)
        {
          std::cout << A.at< double >(m, n)<< " ";
        }
        std::cout << "\n";
      }
      std::cout << "\n";
      std::cout << "\n";

      std::cout << -2*f * cv::determinant(A);

      std::cout << "\n";
      std::cout << "\n";
      //cv::Mat b = cv::Mat::zeros( 8, 1, CV_64F );

      cv::Mat b = cv::Mat::zeros( 3, 1, CV_64F );
      cv::Mat c = cv::Mat::zeros( 3, 1, CV_64F );
      //double lamda_0 = 1024;
      //cv::Mat c = ( Mat_< double >( 8, 1 ) << lamda_0 * p_0_x, lamda_0 * p_0_y, 0, 0, 0, 0, lamda_0 * f, 0/*, 0*/);

      //Ab = c
      solve(A, c, b); //left-hand side, right-hand side, output solution
      for( int k = 0; k < b.rows; k++ )
      {
        std::cout << b.at< double >(k, 0) << std::endl;
      }
      std::cout << "\n";
      std::cout << "\n";

      p_0_x = p_1_x; p_0_y = p_1_y;
      p_1_x = p_2_x; p_1_y = p_2_y;
    }
  }
}

#endif