#include <conio.h> // _getch()
#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <fstream>

using namespace std;
using namespace cv;

/////////////////////////////////////////////////////////////// move to kotsu.h

//#define DRAW_AVERAGE_GRAY

class KOtsu // строит множество разрезов на распределении, оптимизируя критерий отсу для К классов
{
public:
  //int len; // длина распределения
  //int k; // число классов
  vector< long long > distr;   // исходное распределение
  vector< long long > sum;    // sum[i] == sum( distr[0]..distr[i-1] ) // проинтегрированное распределение
  vector< long long > sum_x;  // sum_x[i] == sum( distr[0]*1..distr[i-1]*(i) ) // проинтегрированное (распределение*x)
  vector< long long > sum_xx;  // sum_xx[i] == sum( distr[0]..distr[i-1]*(i*i) ) // проинтегрированное (распределение*x*x)
  ////vector< long long > ssq; // ssq[i] == sum( sq(distr[0])..sq(distr[i-1]) )

  vector< vector< pair< int, double > > > otsu;  // otsu[cuts][pos] = позиция последнего отреза и значение критерия отсу для cuts разрезов
                                    // на фрагменте распределения [0..pos-1] включительно
  // note: разрезы _не_ могут "схлопываться", иначе решение многозначно и экспоненциально по мощн.
  // вернее так -- схлопываться могут, но только с правого края.

  double _weight( int from, int to ) // доля голосов на фрагменте [from, to) взвешенная на фрагменте [0..to)
  {
    double sum_from_to = double(sum[to]-sum[from]);
    assert(sum_from_to >=0);
    if (sum_from_to > 0)
      return sum_from_to / sum[to];
    return 0;
  }

  double _sigma_sq( int from, int to ) // дисперсия на фрагменте [from, to)
  {
    double sum_from_to = double(sum[to]-sum[from]);// число голосующих пациентов
    double sigma_sq =0;
    if (sum_from_to > 0)
    {
      double ave_x = (sum_x[to]-sum_x[from])/sum_from_to;
      sigma_sq = (sum_xx[to]-sum_xx[from])/sum_from_to - ave_x*ave_x;
      assert(sigma_sq >=0);
      //////double ave_sum    = sum_from_to / cnt; // средняя температура по больнице на фрагменте [from..to)
      //////double avesq  = (ssq[to]-ssq[from]) / cnt; // усредненная сумма квадратов "температур" на фрагменте [from..to)
      /////////sigma  = sqrt( avesq - ave*ave ); // среднеквадратичное отклонение на фрагменте [from..to)
      //////sigma  = avesq - ave*ave; // среднеквадратичное отклонение на фрагменте [from..to)
    }
    return sigma_sq;
  }

public:
  KOtsu( int* distribution, int len, int max_k );
  //double getMaxK() { return otsu.size(); } // число классов
  //int getLen() { return sum.size()-1; } // длина распределения
  //double getQuality( int k ) { return otsu[k].back().second; } // число классов
  //double getCut( int k, int pos ) { return otsu[k][pos].first; } // последний отрез справа
  void print()
  {
    
    for (int k=1; k <= int(otsu.size()); k++)
    {
      double min_otsu = otsu[k-1].back().second;
      cout << "k =" << k << " \tmin_otsu = " << min_otsu << " \tk*k*min_otsu=" << k*k*min_otsu 
          << " \tright cut= " << otsu[k-1].back().first << endl;
    }
  }
}; // class KOtsu

KOtsu::KOtsu( int* _distr, int len, int max_k ):
  //len(len), k(max_k),
  distr( _distr, _distr+len),
  sum( len+1, 0 ),
  sum_x( len+1, 0 ),
  sum_xx( len+1, 0 ),
  /////ssq( len+1, 0 ),
  otsu( max_k, vector< pair< int, double > >( len+1 ) )
{
  for (int pos=1; pos<=len; pos++) // pos есмь позиция в распределении, левее которой (не включительно pos) рассчитываем
  {
    sum[pos] = sum[pos-1] + distr[pos-1];
    sum_x[pos] = sum_x[pos-1] + pos*distr[pos-1];
    sum_xx[pos] = sum_xx[pos-1] + pos*pos*distr[pos-1];
    /////ssq[pos] = ssq[pos-1] + distr[pos-1]*(long long)distr[pos-1];
    // заполняем табличку для нулевого количества разрезов
    otsu[0][pos] = make_pair( 0, _weight(0, pos)*_sigma_sq(0, pos) ); // последний отрез [0..pos) и его качество
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
          double _sigsq  = _sigma_sq(last_jump, pos);
          assert(_sigsq >=0);
          double res_otsu = (1-ww)*otsu[cuts-1][last_jump].second + ww * _sigsq; 
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

///////////////////////////////////////////////////////////////
struct Palette
{
  vector< Vec3b > colors;
  Palette( int len=256 ):
  colors(len)
  {
    if (len>0)
    {
      colors[0]=Vec3b(255,255,255);
      for (int i=1; i<len; i++) // идут от светлого к рандомному
      {
        Vec3b& c = colors[i];
        c[0]= rand()%220;  
        c[1]= rand()%220;
        c[2]= rand()%220;
      }
    }
  }
  Palette(vector<int> &cuts, vector<long long> &hist, int len = 256):
  colors(len)
  {
    int ind = len  - 1;
    for (int i = 0; i < cuts.size(); ++i)
    {
      long long sum = 0;
      long long w = 0;
      int ind1 = ind;
      for (;ind >= cuts[i]; --ind)
      {
        w += ind * hist[ind];
        sum += hist[ind];
      }
      long long c1 = w / sum;
     // cout << c1 << endl;
      for (;ind1 >= cuts[i]; --ind1)
      {
        Vec3b& c = colors[i];
        c[0]= c[1] = c[2] = c1;
      }
    }
  }
};

Palette thePalette;

Mat3b colorize( Mat1b& src, vector< int >& index )
{
#ifdef DRAW_AVERAGE_GRAY
  int ind_prev = 0;
  while (ind_prev < index.size())
  {
    int ind = ind_prev;
    while (ind < index.size() && index[ind] == index[ind_prev])
      ++ind;
    int c = (ind_prev + ind) / 2;
    thePalette.colors[index[ind_prev]] = Vec3b(c, c, c);
    ind_prev = ind;
  }
#endif
  Mat3b res( src.rows, src.cols );
  for (int y=0; y<src.rows; y++)
    for( int x=0; x<src.cols; x++)
    {
      int ind = index[ src[y][x] ];
      res[y][x] = thePalette.colors[ind];
    }
  return res;
}



void makePaletteIndex( KOtsu& kotsu, int k, vector< int >& index )
{
  int len = kotsu.distr.size(); 
  index.resize( len );
  int class_num=0;
  int pos = kotsu.otsu[k][len].first; // последний кого надо включить в текущий класс 

  for (int i=index.size()-1; i>=0; i--)
  {
    if (i<pos)
    {
      if (k>0)
        pos = kotsu.otsu[--k][pos].first;
      class_num++;
    }
    index[i] = class_num;
  }
}
string output_png;
string output_hist;
void drawBinarized( KOtsu& kotsu, int classes, Mat1b& img )
{
  vector< int > index;
  makePaletteIndex( kotsu, classes, index );
  Mat3b view_colorized = colorize( img, index );
  imshow( "binary colorized", view_colorized );

   char p1[100];
  sprintf(p1, output_png.c_str(), classes);
  imwrite(p1, view_colorized);
}

string output_path;

void drawKOtsu( KOtsu& kotsu, int k, int thr )
{
  int distr_len = kotsu.distr.size();
  if (distr_len <=0)
    return;
  int distr_max = *max_element( kotsu.distr.begin(), kotsu.distr.end() );
  if (distr_max <=0)
    return;
  double xratio = 512./distr_len;
  double yratio = 512./distr_max;
  int xpix = max( 1, int(xratio) );
  int view_height = int(distr_max*yratio) + 10;
  int view_width = int(distr_len*xratio) + 10;
  Mat view( view_height + 10, view_width + 10,  CV_8UC3 );
  for (int pos=0; pos<distr_len; pos++)
  {
    line( view, Point(int(pos*xratio), view_height), Point( int(pos*xratio), int(view_height-kotsu.distr[pos]*yratio) ), 
      Scalar( 128, 128, 128 ), xpix );
  }

  // отрисовка опенсивишного порога
  line( view, Point(int(thr*xratio), view_height), Point( int(thr*xratio), int(view_height-kotsu.distr[thr]*yratio) ), 
    Scalar( 0, 0, 0 ), 1 );

  int pos = 256;
  cout << "k=" << k << " \tquality=" << (k+1)*(k+1)*kotsu.otsu[k][pos].second;
  char p[100];
  sprintf(p, output_path.c_str(), k);
  ofstream out (p);
 
  vector<int> cuts_;
   

  for ( int cuts = k; cuts>=0; cuts-- )
  {
    pos = kotsu.otsu[cuts][pos].first;
    line( view, Point(int(pos*xratio), view_height), Point( int(pos*xratio), int(view_height-kotsu.distr[pos]*yratio) ), 
      Scalar( 0, 0, 128 ), xpix );
    line( view, Point(int(pos*xratio), 0), Point( int(pos*xratio), int(view_height-kotsu.distr[pos]*yratio-1) ), 
      Scalar( 128, 128, 200 ), xpix );
    cout << " \t" << pos;
    out << pos << " ";
    cuts_.push_back(pos);
  }
  out << endl;
  cout << endl;
  thePalette = Palette(cuts_, kotsu.distr);
  char p1[100];
  sprintf(p1, output_hist.c_str(), k);
  imwrite(p1, view);
  imshow( "kotsu-distr", view );
  

  out.close();
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

  //resize?if (img.rows())

  cout << path << endl;

#if 0
  blur( img, img, Size(3,3) );
#endif

  imshow("kotsu", img);

  int hist[256]={0};
#if 1
  for (int y=0; y<img.rows; y++)
  {
    for (int x=0; x<img.cols; x++)
    {
      hist[ img[y][x] ]++;
    }
  }
#endif

#if 0
  for (int y=1; y<img.rows-1; y++)
  {
    for (int x=1; x<img.cols-1; x++)
    {
      if (    img[y][x] >= img[y][x-1] && img[y][x] >= img[y][x+1]   // soft local maximum by x OR
      ||      img[y][x] >= img[y-1][x] && img[y][x] >= img[y+1][x]   // soft local maximum by y
      )
        hist[ img[y][x] ]++;
      else
      if (    img[y][x] <= img[y][x-1] && img[y][x] <= img[y][x+1]   // soft local maximum by x OR
      ||      img[y][x] <= img[y-1][x] && img[y][x] <= img[y+1][x]   // soft local maximum by y
      )
        hist[ img[y][x] ]++;
    }
  }
#endif

#if 0
  for (int y=1; y<img.rows-1; y++)
  {
    for (int x=1; x<img.cols-1; x++)
    {
      int eps = 0;
      if (    img[y][x] >= img[y][x-1]-eps && img[y][x] >= img[y][x+1]-eps   // soft local maximum by x AND
      &&      img[y][x] >= img[y-1][x]-eps && img[y][x] >= img[y+1][x]-eps   // soft local maximum by y
      )
        hist[ img[y][x] ]++;
      else
      if (    img[y][x] <= img[y][x-1]+eps && img[y][x] <= img[y][x+1]+eps   // soft local minimum by x AND
      &&      img[y][x] <= img[y-1][x]+eps && img[y][x] <= img[y+1][x]+eps   // soft local minimum by y
      )
        hist[ img[y][x] ]++;
    }
  }
#endif

  KOtsu kotsu( hist, sizeof(hist)/sizeof(hist[0]), 255 );

  kotsu.print();

  Mat1b bin_img;
  int thr = (int)threshold( img, bin_img, 0, 255, THRESH_OTSU );
  cout << "threshold( img, bin_img, 0, 255, THRESH_OTSU ) =>" << thr << endl;

  for (int ttt = thr-10; ttt < thr+20; ttt ++)
  {
    double ww = kotsu._weight(ttt, 256);
    double ls = kotsu._sigma_sq(0, ttt );
    double rs = kotsu._sigma_sq(ttt, 256 );
    double res_otsu = (1-ww) * ls + ww * rs;
    cout << "res_otsu(" << ttt << ") =>" << res_otsu << endl;
  }

  int kEscape = 27;
  int kPlus = 43;
  int kEquality = 61; // "=" has same button with "+"
  int kMinus = 45;

  int key=0;
  int classes = 6;
  while (key!=kEscape)
  {
    drawKOtsu( kotsu, classes, thr );
    drawBinarized( kotsu, classes, img );
    key = waitKey(0);
    cout << key << endl;
    if (key == kPlus || key == kEquality)
      classes = std::min( classes+1, int(kotsu.otsu.size())-1 );
    if (key == kMinus)
      classes = std::max( classes-1, 1 );
  }


}


void test_wcv( const char* path, const char *txt_path )
{
  Mat1b img;
  try { 
    img = imread( path, IMREAD_GRAYSCALE ); }
  catch (...) {
    return; }
  if (img.empty())
    return;

  //resize?if (img.rows())

  cout << path << endl;

#if 0
  blur( img, img, Size(3,3) );
#endif

//  imshow("kotsu", img);

  int hist[256]={0};
#if 1
  for (int y=0; y<img.rows; y++)
  {
    for (int x=0; x<img.cols; x++)
    {
      hist[ img[y][x] ]++;
    }
  }
#endif

  KOtsu kotsu( hist, sizeof(hist)/sizeof(hist[0]), 255 );

  ifstream cin(txt_path);
  int n;
  cin >> n;
  vector<int> arr(n + 1);
  for (int i = 0; i < n; ++i)
    cin >> arr[i];
  arr[n] = 256;
  int beg = 0;
  double ans = 0;
  for (int i = 0; i < n + 1; ++i)
  {
    int last_jump = beg;
    int pos = arr[i];
    
    double _sigsq  = kotsu._sigma_sq(last_jump, pos);
    ans +=  (kotsu.sum[pos] - kotsu.sum[last_jump])* _sigsq;
    beg = arr[i];
  }
  
  cout << "Kotsu wcv for input file with " << n << "cuts  result = " << (n + 1) * (n + 1) * ans / (img.cols * img.rows) << endl; 


}

void test_bcv( const char* path, const char *txt_path )
{
  Mat1b img;
  try { 
    img = imread( path, IMREAD_GRAYSCALE ); }
  catch (...) {
    return; }
  if (img.empty())
    return;

  //resize?if (img.rows())

  cout << path << endl;

#if 0
  blur( img, img, Size(3,3) );
#endif

//  imshow("kotsu", img);

  int hist[256]={0};
#if 1
  for (int y=0; y<img.rows; y++)
  {
    for (int x=0; x<img.cols; x++)
    {
      hist[ img[y][x] ]++;
    }
  }
#endif

  KOtsu kotsu( hist, sizeof(hist)/sizeof(hist[0]), 255 );

  ifstream cin(txt_path);
  int n;
  cin >> n;
  vector<int> arr(n + 1);
  for (int i = 0; i < n; ++i)
    cin >> arr[i];
  arr[n] = 256;
  int beg = 0;

  double mu = 0;
  double ans = 0;

  for (int i = 0; i < n + 1; ++i)
  {
    int a = beg;
    int b = arr[i];
    
    double w = 1.0 *(kotsu.sum[b] - kotsu.sum[a]) / (img.cols * img.rows);
    if (w == 0)
      continue;
    double mu_ = (kotsu.sum_x[b] - kotsu.sum_x[a]) / (w * img.cols * img.rows);
    mu += mu_ * w;
    ans += w * mu_ * mu_;
    beg = arr[i];
  }
  
  cout << "Kotsu bcv for input file with " << n << "cuts  result = " << ans - mu * mu << endl; 


}


int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string data = exe + "/../../../testdata/card01.png";
  string txt = exe + "/../../../testdata/input.txt";
  output_path = exe + "/../../../output/output%02d.txt";
  output_png = exe + "/../../../output/kotsu%02d.png";
  output_hist = exe + "/../../../output/hist%02d.png";
  
  //string data = exe + "/../../../testdata/kotsu/greytext.png";
  //string data = exe + "/../../../testdata/kotsu/greytext2.png";
  
  
  //string data = exe + "/../../../testdata/lena.png";
  //string data = exe + "/../../../testdata/peppers256.png";
  //string data = exe + "/../../../testdata/house.png";
  //string data = exe + "/../../../testdata/3096.jpg";
  //string data = exe + "/../../../testdata/372047.jpg"; // soldier
  //string data = exe + "/../../../testdata/35010.jpg"; // butterfly
  //string data = exe + "/../../../testdata/butterfly.png";
  //string data = exe + "/../../../testdata/f-16.png";
  //string data = exe + "/../../../testdata/barbara.png";

  //test_bcv(data.c_str(), txt.c_str());
  //test_wcv(data.c_str(), txt.c_str());

  for (int i=1; i<=2; i++)
  {
    run_kotsu( format( data.c_str(), i ).c_str() );
    if (27==waitKey(0))
      break;
  }


	return 0;
}