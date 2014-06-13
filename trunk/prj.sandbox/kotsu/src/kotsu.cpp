#include <conio.h> // _getch()
#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


class KOtsu // ������ ��������� �������� �� �������������, ����������� �������� ���� ��� � �������
{
public:
  //int len; // ����� �������������
  //int k; // ����� �������
  vector< int > distr;   // �������� �������������
  vector< long long > sum;   // sum[i] == sum( distr[0]..distr[i-1] )
  vector< long long > ssq; // ssq[i] == sum( sq(distr[0])..sq(distr[i-1]) )
  vector< vector< pair< int, double > > > otsu;  // otsu[cuts][pos] = ������� ���������� ������ � �������� �������� ���� ��� cuts ��������
                                    // �� ��������� ������������� [0..pos-1] ������������
  // note: ������� _��_ ����� "������������", ����� ������� ����������� � ��������������� �� ����.
  // ������ ��� -- ������������ �����, �� ������ � ������� ����.

  double _weight( int from, int to ) // ���� ������� �� ��������� [from, to) ���������� �� ��������� [0..to)
  {
    double sum_from_to = double(sum[to]-sum[from]);
    if (sum_from_to > 0)
      return sum_from_to / sum[to];
    return 0;
  }

  double _sigma( int from, int to ) // ��������� �� ��������� [from, to)
  {
    double cnt = to-from; // ����� ���������
    double sum_from_to = double(sum[to]-sum[from]);
    double sigma =0;
    if (cnt >0 && sum_from_to > 0)
    {
      double ave    = sum_from_to / cnt; // ������� ����������� �� �������� �� ��������� [from..to)
      double avesq  = (ssq[to]-ssq[from]) / cnt; // ����������� ����� ��������� "����������" �� ��������� [from..to)
      sigma  = sqrt( avesq - ave*ave ); // ������������������ ���������� �� ��������� [from..to)
    }
    return sigma;
  }

public:
  KOtsu( int* distribution, int len, int max_k );
  //double getMaxK() { return otsu.size(); } // ����� �������
  //int getLen() { return sum.size()-1; } // ����� �������������
  //double getQuality( int k ) { return otsu[k].back().second; } // ����� �������
  //double getCut( int k, int pos ) { return otsu[k][pos].first; } // ��������� ����� ������
  void print()
  {
    
    for (int k=0; k < int(otsu.size()); k++)
    {
      cout << "k =" << k << " minval = " << otsu[k].back().second << " right cut = " << otsu[k].back().first << endl;
    }
  }


};

void drawKOtsu( KOtsu& kotsu, int k )
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

  int pos = 256;
  for ( int cuts = k; cuts>=0; cuts-- )
  {
    pos = kotsu.otsu[cuts][pos].first;
    line( view, Point(int(pos*xratio), view_height), Point( int(pos*xratio), int(view_height-kotsu.distr[pos]*yratio) ), 
      Scalar( 0, 0, 128 ), xpix );
    line( view, Point(int(pos*xratio), 0), Point( int(pos*xratio), int(view_height-kotsu.distr[pos]*yratio-1) ), 
      Scalar( 128, 128, 200 ), xpix );
  }

  imshow( "kotsu-distr", view );
}

KOtsu::KOtsu( int* distr, int len, int max_k ):
  //len(len), k(max_k),
  distr( distr, distr+len),
  sum( len+1, 0 ),
  ssq( len+1, 0 ),
  otsu( max_k, vector< pair< int, double > >( len+1 ) )
{
  for (int pos=1; pos<=len; pos++) // pos ���� ������� � �������������, ����� ������� (�� ������������ pos) ������������
  {
    sum[pos] = sum[pos-1] + distr[pos-1];
    ssq[pos] = ssq[pos-1] + distr[pos-1]*(long long)distr[pos-1];
    // ��������� �������� ��� �������� ���������� ��������
    otsu[0][pos] = make_pair( 0, _weight(0, pos)*_sigma(0, pos) ); // ��������� ����� [0..pos) � ��� ��������
  }
  for (int cuts =1; cuts < max_k; cuts++) // cuts - ���������� ��������, �� �� 1 ������ ��� ����� ������� max_k
  { // ������ ��������� ������� ��� ��� ������ �������, ���������� ����� ������� pos
    // ��������� ������� ��� ��������� [0..pos) ��� �-1 ������� ��� ��������� � dp[k-1][pos]
    otsu[cuts][0] = otsu[cuts-1][0];
    for (int pos=1; pos<=len; pos++)
    {
      pair< int, double > best_cut = make_pair( pos, otsu[cuts-1][pos].second );// ������ � ���� ��� �������� ������ � ������ �� ���������� k-1 (??)
      for (int last_jump=pos-1; last_jump >=0; last_jump--)
      { // ���� ����������� jump ����� ��� ���������� �������
        double ww = _weight(last_jump, pos);
        if (ww>0)
        {
          double res_otsu = (1-ww)*otsu[cuts-1][last_jump].second + ww * _sigma(last_jump, pos);
          if (res_otsu < best_cut.second) // ��� <= ??? ������ ���� ���������
          {
            best_cut.first = last_jump;
            best_cut.second = res_otsu;
          }
        }
      }
      otsu[cuts][pos] = best_cut; // ��� ))
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

  //resize?if (img.rows())

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

  KOtsu kotsu( hist, sizeof(hist)/sizeof(hist[0]), 30 );

  kotsu.print();

  Mat1b bin_img;
  int thr = (int)threshold( img, bin_img, 0, 255, THRESH_OTSU );
  cout << "threshold( img, bin_img, 0, 255, THRESH_OTSU ) =>" << thr << endl;

  for (int ttt = thr-10; ttt < thr+20; ttt ++)
  {
    double ww = kotsu._weight(ttt, 256);
    double ls = kotsu._sigma(0, ttt );
    double rs = kotsu._sigma(ttt, 256 );
    double res_otsu = (1-ww) * ls + ww * rs;
    cout << "res_otsu(" << ttt << ") =>" << res_otsu << endl;
  }

  int kEscape = 27;
  int kPlus = 43;
  int kEquality = 61; // "=" has same button with "+"
  int kMinus = 45;

  int key=0;
  int classes = 2;
  while (key!=kEscape)
  {
    drawKOtsu( kotsu, classes );
    key = waitKey(0);
    cout << key << endl;
    if (key == kPlus || key == kEquality)
      classes = std::min( classes+1, int(kotsu.otsu.size())-1 );
    if (key == kMinus)
      classes = std::max( classes-1, 1 );
  }


}


int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string data = exe + "/../../../testdata/card%02d.png";

  for (int i=1; i<=2; i++)
  {
    run_kotsu( format( data.c_str(), i ).c_str() );
    if (27==waitKey(0))
      break;
  }

	return 0;
}