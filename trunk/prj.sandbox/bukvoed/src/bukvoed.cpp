// bukvoed
#include "precomp.h"
//
//#include <conio.h>
//#include <cassert>
//#include <climits>
//
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <algorithm>
//
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv2/imgproc/imgproc.hpp"

#include "cover_net.h"
#include "ticker.h"
#include "pagedata.h"

using namespace std;
using namespace cv;


static void open_close_vertical( Mat& in, Mat& ou, bool inverted = false )
{
  int an = 1;
  int element_shape = MORPH_RECT;
  Mat element = getStructuringElement(element_shape, Size(1, an*2+1), Point(0,2) );
  Mat tmp( in.rows, in.cols, in.type() );
  if (inverted)
  {
    erode( in, tmp, element, Point( 0, 2 ) );
    dilate( tmp, ou, element, Point( 0, 2 ) );
  }
  else
  {
    dilate( in, tmp, element, Point( 0, 2 ) );
    erode( tmp, ou, element, Point( 0, 2 ) );
  }

  //imshow( "before_open_close", in );
  //imshow( "after__open_close", ou );
  //waitKey(0);

}

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////



struct CoverPoint
{
  int iPage; // номер страницы, индекс к pages[]
  int iCC;  // номер компоненты связности, индекс к pages[iPage].cc[]
  CoverPoint( int iPage, int iCC ): iPage( iPage ), iCC( iCC ) {}
  bool operator == ( const CoverPoint & other ) const { return iPage == other.iPage && iCC == other.iCC; }
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
  void Bukvoed::makeIndex();
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


void Bukvoed::makeIndex()
{
  ///// make_index
  vector< pair< pair< int, int >, int > > index; // < < sphere_level, count_of_points >, sphere_index >
  for (int i=0; i<cvnet.getSpheresCount(); i++)
  {
    const CoverSphere< CoverPoint > & sphere = cvnet.getSphere(i);
    if (1)//(sphere.level == 16)
      index.push_back( make_pair( make_pair( sphere.level, sphere.points ), i ) );
  }
  sort( index.rbegin(), index.rend() );
  //////////////////
  int level=100;
  for (int i=0; i<index.size(); i++)
  {
    if (index[i].first.first < level)
    {
      level = index[i].first.first; 
      cout << "\n\nlevel= " << level << endl;
    }
    printf( "\tpnts=%d ", index[i].first.second );
  }

}

int Bukvoed::browse()
{

  int iPage = 0;
  if (pages.size() > 0) do  
  {
    PageData& pd =pages[iPage];
    Mat draw_bin = pd.src_binarized.clone();
    Mat draw_src = pd.src.clone();
    int ratio =3;
    resize( draw_bin, draw_bin, Size(), ratio, ratio, INTER_AREA );
    resize( draw_src, draw_src, Size(), ratio, ratio, INTER_AREA );
    for (int iCC =0; iCC<pd.cc.size(); iCC++)
    {
      CCData& ccd = pd.cc[iCC];
      rectangle( draw_bin, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), Scalar( 128, 0, 0 )  );
      rectangle( draw_src, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), Scalar( 255, 0, 0 )  );
    }

    imshow("comps_bin", draw_bin); 
    imshow("comps_src", draw_src); 
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

#if 0
  // set roi
  int dx =10;
  int dy = 5;
  //bukvoed.ROI = Rect( 300-dx, 263-dy, 350+2*dx, 51+2*dy );// address
  //bukvoed.ROI = Rect( 304-dx, 200-dy, 322+2*dx, 109+2*dy ); // name+address
  //bukvoed.ROI = Rect( 392-dx, 477-dy, 279+2*dx, 39+2*dy ); // dd
  //bukvoed.ROI = Rect( 765-dx, 1751-dy, 847+2*dx, 315+2*dy ); // bankimage
#endif

  //bukvoed.addPage( "/images/4.png" );

  Ticker t;
  /////bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0001_003_.jpg" ); /// <<< ломает дерево: все липнет к первым двум уровням, ошибка какая-то
  //bukvoed.addPage( "/images/niblack/board1.png" ); 
  bukvoed.addPage( "/images/niblack/board1_snippet.png" ); 
  //bukvoed.addPage( "/images/bankimage004.jpg" ); 
#if 0
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0003_005_.jpg" );
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0004_005_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0005_009_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0006_009_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0001_004_.jpg" ); 
  bukvoed.addPage( "/testdata/idcard/snippets/US/CA/DL03/ID_US_CA_DL03_0007_005_.jpg" ); 
#endif

  cout << "addPages ... " << t.msecs() << " milliseconds" << endl;


  bukvoed.makeIndex();
  bukvoed.browse();

  return 0;

}
