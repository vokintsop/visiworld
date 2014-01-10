// bukvoed
#include <conio.h>
#include <cassert>
#include <climits>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

#include "cover_net.h"
#include "ticker.h"

using namespace std;
using namespace cv;



static void dilate1( Mat& in, Mat& ou, bool inverted = false )
{
  int an = 1;
  int element_shape = MORPH_CROSS; // MORPH_RECT;
  Mat element = getStructuringElement(element_shape, Size(an*2+1, an*2+1), Point(an, an) );
  if (!inverted)
    dilate( in, ou, element, Point( 1, 1 ) );
  else
    erode( in, ou, element, Point( 1, 1 ) );

  //imshow( "before_dilation", in );
  //imshow( "after_dilation", ou );
  //waitKey(0);

}


Mat make_labels( const cv::Mat &bw, int& label_index, vector< cv::Rect >& rects )
{
  Ticker t;  
  Mat labels;
  bw.convertTo(labels, CV_32SC1);
  label_index = 1; 
  rects.clear();
  rects.push_back( Rect() );
  for(int y=0; y < labels.rows; y++)
  {
    int *row = (int*)labels.ptr(y);
    for(int x=0; x < labels.cols; x++) 
    {
      if( row[x] != 255 )
          continue;
      cv::Rect rect;
      cv::floodFill(labels, cv::Point(x,y), label_index, &rect, 0, 0, 4);
      label_index++;
      rects.push_back( rect );
      assert( label_index == rects.size() );

      if (label_index == 255)
      {
        label_index++;
        rects.push_back( Rect() );
      }
    }
  }
  cout << "make_labels() ... " << t.msecs() << " milliseconds" << endl;
  return labels;
}


class CCData
{
public:
  int minx, miny, maxx, maxy; // коробка, включительно
  int height() { return maxy-miny+1; }
  int width() { return maxx-minx+1; }
  double xc, yc; // центр масс
  double sum_f;
  double sum_fx,sum_fy;
  double sum_fxx, sum_fxy, sum_fyy;
  CCData():
    minx( INT_MAX ), miny( INT_MAX ), maxx( INT_MIN ), maxy(INT_MIN ),
    sum_f(0.), sum_fx(0.), sum_fy(0.), sum_fxx(0.), sum_fxy(0.), sum_fyy(0.)
  {}
  void add( int x, int y, double f = 1. )
  {
    minx = min( x, minx );     miny = min( y, miny ); 
    maxx = max( x, maxx );     maxy = max( y, maxy ); 
    sum_f += f;
    sum_fx += f*x; 
    sum_fy += f*y;
    sum_fxx += f * x * x;  
    sum_fxy += f * x * y;
    sum_fyy += f * y * y;
  }
  void fix()
  {
    xc = sum_fx / sum_f;
    yc = sum_fy / sum_f;
    /*
    // sum_fxx_ == sum( f * (x-xc) * (x-xc) ) == sum( f*x*x - 2*f*x*xc + f*xc*xc ) ==
    // == sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc
    double sum_fxx_ = sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc;
    double sum_fyy_ = sum_fyy - 2*sum_fy * yc  + sum_f * yc * yc;

    // fxy_ == sum( f * (x-xc) * (y-yc) ) == sum( f*x*x - f*x*yc - f*y*xc + f*xc*yc ) ==
    // == sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc
    double sum_fxy_ = sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc;

    double ds = sum_fxx_ - sum_fyy_;
    double phi_rad =  ( abs( ds ) <= 0.000001 ) ? 0. 
      : 0.5 * atan( 2*sum_fxy_ / ds );
      //: 0.5 * atan2( 2*sum_fxy_ , ds );

    double I1 = 0.5*( sum_fxx_ + sum_fyy_  + sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));
    double I2 = 0.5*( sum_fxx_ + sum_fyy_  - sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));

    double phi_degree = phi_rad * 360./ (CV_PI*2);
    */
  }
};



struct PageData
{
  Mat1b src; // входное изображение, ч-б, 0-черное (сигнал), 255-белое (фон)
  Mat1b src_dilated; // размазанное входное
  Mat1b src_binarized; // 0-background, 255-foreground
  Mat1i labels; // карта компонент связности, [2...labels.size()-1] ==> cc[]
  vector< CCData > cc; // информация о компонентах
  PageData(){}
  PageData( const char* filename ) {    compute(filename);  }
  bool compute( const char* filename );
};

#include "niblack.h"
bool PageData::compute( const char* filename )
{
  src = imread( filename, IMREAD_GRAYSCALE );
  dilate1( src, src_dilated, true );

  //src_binarized();
  double thresh = threshold( src, src_binarized, 128., 255., THRESH_BINARY_INV | CV_THRESH_OTSU );
  //int res = niblack( src, src_binarized, true );
  
#if 0
  imshow( "src", src );
  imshow( "src_binarized", src_binarized );
  waitKey(0);
#endif

  int label_index=0;
  vector< Rect > rects;
  labels = make_labels( src_binarized, label_index, rects );

  cc.clear(); 
  cc.resize( label_index );
  for(int y=0; y < labels.rows; y++)
  {
    int *row = (int*)labels.ptr(y);
    for(int x=0; x < labels.cols; x++) 
    {
      if( row[x] <= 1 )
          continue;
      cc[ row[x] ].add( x, y );
    }
  }
  for (int i=1; i< int(cc.size()); i++ ) // <<<<<<<<<<< 2 comp -> gpf
  {
    CCData& ccd = cc[i];
    ccd.fix();
#if 0 // draw rects
    rectangle( src, Point( ccd.minx, ccd.miny ), Point( ccd.maxx, ccd.maxy ), Scalar( 128, 0, 0 ) );
#endif
  }
  //imshow( "comps", src );
  //waitKey(0);

  //cout << labels;
  return true;
}

struct CoverPoint
{
  int iPage; // номер страницы, индекс к pages[]
  int iCC;  // номер компоненты связности, индекс к pages[iPage].cc[]
  CoverPoint( int iPage, int iCC ): iPage( iPage ), iCC( iCC ) {}
};

inline bool more_128( int x, int y, Mat1b& m ) // white
{
  if ( x < m.cols && x>=0 && y < m.rows && y >= 0 )
    return m[y][x] > 128;
  return true; // снаружи считаем все белым
  //return x < m.cols && x>=0 && y < m.rows && y >= 0 && m[y][x] > 128;
}

////////////////////////////////////////////////////////////////////////////////////
class MetrСС // A intersect dilated(B) + B intersect dilated(A)
{
public:
  long long counter; // 
  vector< PageData >& pages;
  MetrСС( vector< PageData >& pages ): pages(pages), counter(0){}

  double computeDistance( const CoverPoint& cp1,  const CoverPoint& cp2 )  // индексы к сс[]
  {
    PageData& pd1 = pages[cp1.iPage];    
    PageData& pd2 = pages[cp2.iPage];
    CCData& ccd1 = pd1.cc[cp1.iCC];            
    CCData& ccd2 = pd2.cc[cp2.iCC];

    double dst=0;
    int dx = cvRound( ccd2.xc - ccd1.xc );
    int dy = cvRound( ccd2.yc - ccd1.yc );
    for ( int y=ccd1.miny; y<ccd1.maxy; y++ )
    {
      for ( int x=ccd1.minx; x<ccd1.maxx; x++ )
      {
        if (pd1.labels[y][x] == cp1.iCC) // сигнал
        {
          int qqq = pd1.src[y][x];
          //?assert( pd1.src[y][x] < 128 );  // буквы -- сигнал -- черные -- 00.
          if (more_128( x+dx, y+dy, pd2.src_dilated ))
            dst++;
        }
      }
    }

    for ( int y=ccd2.miny; y<ccd2.maxy; y++ )
    {
      for ( int x=ccd2.minx; x<ccd2.maxx; x++ )
      {
        if (pd2.labels[y][x] == cp2.iCC) // сигнал
        {
          //?assert( pd2.src[y][x] < 128 );
          if (more_128( x-dx, y-dy, pd1.src_dilated ))
            dst++;
        }
      }
    }

    counter++;
    return dst;
  }
};

class Bukvoed
{
  vector< PageData > pages;
  MetrСС ruler;
  CoverNet< CoverPoint, MetrСС > cvnet;
public:
  Bukvoed(): ruler( pages ), cvnet( &ruler, 1000000, 1 ){};
  int Bukvoed::addPage( const char* page_file );
  int Bukvoed::browse();
};

int Bukvoed::addPage( const char* page_file )
{
  PageData pd( page_file );
  pages.push_back( pd );
  int iPage = pages.size()-1;


  {
    Ticker t;
    for (int iCC=1; iCC<int(pages[iPage].cc.size()); iCC++) // <<<<<<<<<<< 2 comp -> gpf
    {
      CoverPoint p( iPage, iCC );
      if (pages[iPage].cc[iCC].sum_f <= 20)
        continue;
      if (pages[iPage].cc[iCC].height() > 40 || pages[iPage].cc[iCC].width() > 40)
        continue;
      cvnet.insert(p);
      //cvnet.reportStatistics( 0, 3 ); 

    }

    double ms = t.msecs();
    cout << "MetrCC metrics (simple L1):" << endl;
    cvnet.reportStatistics( 0, 3 ); 
    cout << "Build time = " << ms/1000 << " seconds" << endl;
  }

  return 0;
}

const int KEY_ESCAPE =27;
const int KEY_ENTER =13;
const int KEY_LEFT_ARROW  =2424832;
const int KEY_RIGHT_ARROW  =2555904;
const int KEY_PAGE_UP =2162688;
const int KEY_PAGE_DOWN =2228224;
const int KEY_SPACE =32;
const int KEY_GREY_PLUS =43;
const int KEY_GREY_MINUS =45;
const int KEY_NO_KEY_PRESSED =-1;  // after positive delay no key pressed -- process next image


int Bukvoed::browse()
{


  ///// make_index
  vector< pair< pair< int, int >, int > > index; // < < sphere_level, count_of_points >, sphere_index >
  for (int i=0; i<cvnet.getSpheresCount(); i++)
  {
    const CoverSphere< CoverPoint > & sphere = cvnet.getSphere(i);
    if (sphere.level == 16)
      index.push_back( make_pair( make_pair( sphere.level, sphere.points ), i ) );
  }
  sort( index.rbegin(), index.rend() );
  //////////////////



  int iPage = 0;
  if (pages.size() > 0) do  
  {
    PageData& pd =pages[iPage];
    Mat draw = pd.src.clone();
    for (int iCC =0; iCC<pd.cc.size(); iCC++)
    {
      CCData& ccd = pd.cc[iCC];
      rectangle( draw, Point( ccd.minx, ccd.miny ), Point( ccd.maxx, ccd.maxy ), Scalar( 255, 0, 0 )  );
    }

    imshow("comps", draw); 
    int key = waitKey(0);
    switch (key)
    {
    case KEY_ESCAPE:
      return 0;
    case KEY_RIGHT_ARROW:
      iPage = ((iPage+1) % int( pages.size()) );
      break;
    case KEY_LEFT_ARROW:
      iPage = ((pages.size()+ iPage-1) % int( pages.size()) );
      break;

    }

  } while (1);
  return 0;
}


int run_bukvoed( int argc, char* argv[] )
{
  Bukvoed bukvoed;
  //bukvoed.addPage( "/images/4.png" );

  Ticker t;
  /////bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0001_003_.jpg" ); /// <<< ломает дерево: все липнет к первым двум уровням, ошибка какая-то
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0003_005_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0004_005_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0005_009_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0006_009_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0001_004_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0007_005_.jpg" ); 

  cout << "addPages ... " << t.msecs() << " milliseconds" << endl;


  bukvoed.browse();

  return 0;

}
