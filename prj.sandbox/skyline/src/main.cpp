#include <conio.h> // _getch()
#include <iostream>
#include <fstream>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


void skyline_single_level( Mat1b& grey, Mat& draw )
{
  Mat1i sum;  integral( grey, sum );
  double tot = sum[sum.rows-1][sum.cols-1];
  double mots = -1;  int my = -1; // record max
  vector< double > q_otsu(sum.rows, 0);

  for (int y=0;y<sum.rows;y++)
  {
    double u = sum[y][sum.cols-1]; int nu = y;
    double d = tot - u; int nd = sum.rows-y;
    double ots_u = nu ? u*u/nu : 0;
    double ots_d = nd ? d*d/nd : 0;
    double ots = ots_u + ots_d;
    
    if (ots > mots)
      mots = ots,  my = y;

    q_otsu[y] = ots;
  }
  line( draw, Point(0, my), Point( grey.cols-1, my ), Scalar( 0, 255, 255 ), 3 );

#if 1
  double minq = mots;
  for (int y=0; y<sum.rows; y++)
    minq= min( minq, q_otsu[y] );

  for (int y=0; y<sum.rows; y++)
  {
    circle( draw, Point(draw.cols-(1+q_otsu[y]-minq)*100./(1+mots-minq), y), 3, Scalar( 100, 0, 255 ), 1 );
  }
#endif

  imshow( "single_level", draw );
}

void skyline_tiled( Mat1b& grey, Mat& draw )
{
  Mat1i sum;  integral( grey, sum );
  int xx = sum.cols/10;
  for (int x=0; x<sum.cols-xx; x++)
  {
    double mots = -1;  int my = -1;
    double tot = sum[0][x] + sum[sum.rows-1][x+xx] - sum[sum.rows-1][x] - sum[0][x+xx];
    for (int y=0;y<sum.rows;y++)
    {
      double u = sum[0][x] + sum[y][x+xx] - sum[y][x] - sum[0][x+xx]; int nu = y;
      double d = tot - u; int nd = sum.rows-y;
      double ots_u = nu ? u*u/nu : 0;
      double ots_d = nd ? d*d/nd : 0;
      double ots = ots_u + ots_d;
      
      if (ots > mots)
        mots = ots, my = y;
    }
    line( draw, Point(x, my), Point( x+xx, my ), Scalar( 0, 255, 255 ), 3 );
  }
  imshow( "tiled", draw );

}

void skyline_dp( Mat1b& grey, Mat& draw )
{
  Mat1i sum;  integral( grey, sum );

  // NOTE:
  // разрезы идут между пиксел€ми. разрез из точки (1 1) отрезает единственный левый верхний пиксель с координатами (0 0)
  // соответственно, матрицы динамического программировани€ имеют размеры на 1 больше исходной картинки
  // аналогично матрице сумм
  // левый столбец и верхн€€ строка -- нулевые. избыточна€ пам€ть, упрощение кода.

  Mat1i sum_up( sum.rows, sum.cols, 0 ); // сумма интенсивностей над разрезом
  Mat1i cnt_up( sum.rows, sum.cols, 0 ); // количество пикселей над разрезом
  Mat1i y_prev( sum.rows, sum.cols, 0 ); // переход к предыдущему столбцу
  Mat1d q_otsu( sum.rows, sum.cols, 0. ); // переход к предыдущему столбцу

  double mots = -1;  int my = -1; // рекорд по столбцу
  for (int x=1; x<sum.cols; x++) // расчет очередного столбца
  {
    double sum_tot = sum[sum.rows-1][x]; // обща€ сумма слева, включа€ рассчитываемый столбец
    double cnt_tot = sum.rows*x; // общее количество слева, включа€ рассчитываемый столбец
    mots = -1;  my = -1; // рекорд по столбцу
    for (int y=0;y<sum.rows;y++) // оцениваем разрез из (x,y)
    {
      const int k = 2;
      int y1 = max( 0, y-k ); int y2 = min( sum.rows-1, y+k );
      double _mots = -1;  int _my = -1; // рекорд по ребрам из (x,y)
      for ( int yy = y1; yy <= y2; yy++)
      { // расчитываем путь с ребром (x,y)--(x-1,yy)
        double u =  sum_up[yy][x-1]; // сумма по пути над лучшим разрезом из (x-1, yy)
        u += sum[y][x] - sum[y][x-1]; // сумма над столбцом слева от (x,y)
        int nu = cnt_up[yy][x-1];
        nu += y; // число пикселей
        double d = sum_tot - u; int nd = cnt_tot-nu;
        double ots_u = nu ? u*u/nu : 0;
        double ots_d = nd ? d*d/nd : 0;
        double ots = ots_u + ots_d;
        
        if (ots > _mots)
          _mots = ots, _my = yy; 
      }
      assert( _my >=0 );
      assert( _my < sum.rows );

      y_prev[y][x] = _my;
      sum_up[y][x] = sum_up[_my][x-1] // сумма по пути над разрезом
                  + sum[y][x] - sum[y][x-1]; // сумма над столбцом
      cnt_up[y][x] = cnt_up[_my][x-1] + y; // число пикселей
      q_otsu[y][x] = _mots;

      if (_mots > mots) // ищем уже глобальный максимум по столбцу
        mots = _mots, my = y;
    }
  }

  double minq = mots;
  for (int y=0; y<sum.rows; y++)
    minq= min( minq, q_otsu[y][sum.cols-1] );

  for (int y=0; y<sum.rows; y++)
  {
    circle( draw, Point(draw.cols-(1+q_otsu[y][sum.cols-1]-minq)*100./(1+mots-minq), y), 2, Scalar( 255, 0, 255 ), 1 );
  }

  for (int x=sum.cols-1; x>=0; x--)
  {
    circle( draw, Point(x, my), 2, Scalar( 255, 255, 0 ), 1 );
    my = y_prev[my][x];
  }

  imshow( "dp", draw );

}

bool process_image_file( const string& image_file_name )
{
  cout << image_file_name << endl;
  Mat3b bgr = imread( image_file_name );
  vector<Mat1b> channels;
  split( bgr, channels );

  Mat1b grey = channels[0]; ///imread( image_file_name, IMREAD_GRAYSCALE );
  imshow("blue", channels[0]);
  //imshow("gre", channels[1]);
  //imshow("red", channels[2]);
  Mat draw = bgr.clone();

  blur( grey, grey, Size( 5, 3 ) );
  int lev = grey.rows/10; // осветл€ем заведомо небо, 10% вверх -- накапливаем минимум
  for (int x=0; x<grey.cols; x++)
    for (int y=lev+1; y<grey.rows; y++)
      grey[y][x] = min( grey[y][x], grey[y-1][x] );

  for (int x=0; x<grey.cols; x++) // затемн€ем вниз, чтобы избавитьс€ от белых домов и тп
    for (int y=lev-1; y>=0; y--)
      grey[y][x] = max( grey[y][x], grey[y+1][x] );

  imshow( "preprocessed", grey );

  skyline_single_level( grey, draw );
  //skyline_tiled( grey, draw );
  skyline_dp( grey, draw );

  if (27==waitKey(0))
    return false;
  return true;
}

int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string foldername = exe + "/../../../testdata/skyline/";
  string lst = exe + "/../../../testdata/skyline/skyline.lst";

  ifstream inn( lst.c_str() );
  if (!inn.is_open())
    cout << "Can't open " << lst;
  else
  {
    while ( inn.good() )
    {
      string filename;
      inn >> filename;
      if (filename.empty())
        break;
      string pathname = foldername+filename;
      if (!process_image_file( pathname ))
        break;
    }
  }
	return 0;
}