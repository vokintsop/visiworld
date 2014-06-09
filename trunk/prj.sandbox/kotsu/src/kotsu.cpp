#include <conio.h> // _getch()
#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

class KOtsu // строит множество разрезов на распределении, оптимизируя критерий отсу для К классов
{
  vector< long long > sum;   // sum[i] == sum( distr[0]..distr[i-1] )
  vector< long long > ssq; // ssq[i] == sum( sq(distr[0])..sq(distr[i-1]) )
  vector< vector< pair< int, double > > > otsu;  // otsu[cuts][pos] = позиция последнего отреза и значение критерия отсу для cuts разрезов
                                    // на фрагменте распределения [0..pos-1] включительно
  // note: разрезы _не_ могут "схлопываться", иначе решение многозначно и экспоненциально по мощн.
  // вернее так -- схлопываться могут, но только с правого края.

  double _weight( int from, int to ) // доля голосов на фрагменте [from, to) взвешенная на фрагменте [0..to)
  {
    double sum_from_to = sum[to]-sum[from];
    if (sum_from_to > 0)
      return sum_from_to / sum[to];
    return 0;
  }

  double _sigma( int from, int to ) // дисперсия на фрагменте [from, to) взвешенная весом [from..to) на фрагменте [0..to)
  {
    double cnt = to-from; // число пациентов
    double sum_from_to = sum[to]-sum[from];
    double sigma =0;
    if (cnt >0 && sum_from_to > 0)
    {
      double ave    = sum_from_to / cnt; // средняя температура по больнице на фрагменте [from..to)
      double avesq  = (ssq[to]-ssq[from]) / cnt; // усредненная сумма квадратов "температур" на фрагменте [from..to)
      sigma  = sqrt( avesq - ave*ave ); // среднеквадратичное отклонение на фрагменте [from..to)
    }
    return sigma;
  }

public:
  KOtsu( int* distribution, int len, int max_k );

};

KOtsu::KOtsu( int* distr, int len, int max_k ):
  sum( len+1, 0 ),
  ssq( len+1, 0 ),
  otsu( max_k, vector< pair< int, double > >( len+1 ) )
{
  for (int pos=1; pos<=len; pos++) // pos есмь позиция в распределении, левее которой (не включительно pos) рассчитываем
  {
    sum[pos] = sum[pos-1] + distr[pos-1];
    ssq[pos] = ssq[pos-1] + distr[pos-1]*(long long)distr[pos-1];
    // заполняем табличку для нулевого количества разрезов
    otsu[0][pos] = make_pair( 0, _weight(0, pos)*_sigma(0, pos) ); // последний отрез [0..pos) и его качество
  }
  for (int cuts =1; cuts < max_k; cuts++) // cuts - количество разрезов, их на 1 меньше чем число классов max_k
  { // оценим наилучший вариант для еще одного разреза, сделанного после позиции pos
    // наилучшее решение для фрагмента [0..pos) для к-1 разреза уже посчитано в dp[k-1][pos]
    otsu[cuts][0] = otsu[cuts-1][0];
    for (int pos=1; pos<=len; pos++)
    {
      pair< int, double > best_cut = make_pair( pos, otsu[cuts-1][pos].second );// начнем с того что схлопнем разрез с правым из предыдущих k-1 (??)
      for (int last_jump=pos-1; last_jump >=0; last_jump--)
      { // ищем оптимальный jump влево для последнего разреза
        double ww = _weight(last_jump, pos);
        if (ww>0)
        {
          double res_otsu = (1-ww)*otsu[cuts-1][last_jump].second + ww * _sigma(last_jump, pos);
          if (res_otsu < best_cut.second) // или <= ??? вправо надо прижимать
          {
            best_cut.first = last_jump;
            best_cut.second = res_otsu;
          }
        }
      }
      otsu[cuts][pos] = best_cut; // уфф ))
    }
  }

}


void run_kotsu( const char* path )
{
  Mat1b img;
  try { 
    img = imread( path, IMREAD_GRAYSCALE ); }
  catch (...) {
    return; }
  if (img.empty())
    return;

  cout << path << endl;

  imshow("kotsu", img);

  int hist[256]={0};
  for (int y=0; y<img.rows; y++)
  {
    for (int x=0; x<img.cols; x++)
    {
      hist[ img[y][x] ]++;
    }
  }
  cout << "--- hor ---\n";

  KOtsu kotsu( hist, sizeof(hist)/sizeof(hist[0]), 5 );

}


int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string data = exe + "/../../../testdata/overcom/overcom01/IMG_3463_%02d.jpg";

  for (int i=1; i<100; i++)
  {
    run_kotsu( format( data.c_str(), i ).c_str() );
    if (27==waitKey(0))
      break;
  }

	return 0;
}