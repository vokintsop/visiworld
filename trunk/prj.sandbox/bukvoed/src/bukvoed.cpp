// bukvoed
#include "precomp.h"

#include "bukvoed.h"

#include "cover_net/cover_net.h"
#include "ocvutils/ticker.h"
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

int Bukvoed::addPage( const char* page_file, Rect roi )
{
  PageData pd;
  pd.ROI = roi;
  try {
    pd.compute( page_file );
  } catch (...) {
    cout << "Can't add page " << page_file << endl;
    return -1;
  }

  //for (int y=0; y<pd.src.rows; y++) for (int x=0; x<pd.src.cols-45; x++)
  //  if (pd.src[y][x] != pd.src[y][x+45])
  //    cout << "diff at: y=" << y << " x=" << x << endl;

  pages.push_back( pd );

  int iPage = pages.size()-1;

  cout << "Adding page #" << iPage << endl;
  cout << "Total comps count = " << pages[iPage].cc.size() << endl;


  {
    Ticker t;
    for (int iCC=1; iCC<int(pages[iPage].cc.size()); iCC++) // <<<<<<<<<<< 2 comp -> gpf
    {
      CoverPoint p( iPage, iCC );
      //if (pages[iPage].cc[iCC].sum_f <= 20)
      //  continue;
      //if (pages[iPage].cc[iCC].height() > 60 || pages[iPage].cc[iCC].width() > 60)
      //  continue;
      cvnet.insert(p);
      //cvnet.reportStatistics( 0, 3 ); 

    }

    double ms = (double)t.msecs();
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
  for (int i=0; i<(int)index.size(); i++)
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
    Mat draw_dil = pd.src_dilated.clone();

    double ratio = min( 4., 600./pd.src.rows );
    resize( draw_bin, draw_bin, Size(), ratio, ratio, INTER_AREA );
    resize( draw_src, draw_src, Size(), ratio, ratio, INTER_AREA );
    resize( draw_dil, draw_dil, Size(), ratio, ratio, INTER_AREA );
    for (int iCC =0; iCC<(int)pd.cc.size(); iCC++)
    {
      CCData& ccd = pd.cc[iCC];
#if 0
      rectangle( draw_bin, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), Scalar( 128, 0, 0 )  );
      rectangle( draw_src, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), Scalar( 255, 0, 0 )  );
      rectangle( draw_dil, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), Scalar( 255, 0, 0 )  );
#endif
    }

    imshow("comps_bin", draw_bin); 
    imshow("comps_src", draw_src); 
    imshow("comps_src_dilated", draw_dil); 
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
#endif

  Ticker t;
  //bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_zero_norm.png").c_str() ); 
  //bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_zero_bold.png").c_str() ); 
#if 0
  string filename = string(argv[0]) +"/../../../testdata/bukvoed/arial_digits_norm.png";
  Rect roi; // полная страница
  //Rect roi = Rect( 29, 15, 103, 133 ); // 00 11 -- четыре символа -- два ноля и две единицы -- ok, две сферы в ответе с 12 по 20 уровень
  // <<<>>>> Rect roi = Rect( 29, 15, 103, 641 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  //Rect roi = Rect( 29, 208, 103, 64 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  bukvoed.addPage( filename.c_str(), roi ); 
#endif

#if 0
  string filename = string(argv[0]) +"/../../../testdata/bukvoed/arial_latin_capital_norm.png";
  // 13 sizes x 26 letters == 338 -- 6,7,8,9,10,11,12,14,16,18,20,22,24
  Rect roi; // полная страница
  //Rect roi = Rect( 29, 15, 103, 133 ); // 00 11 -- четыре символа -- два ноля и две единицы -- ok, две сферы в ответе с 12 по 20 уровень
  // <<<>>>> Rect roi = Rect( 29, 15, 103, 641 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  //Rect roi = Rect( 29, 208, 103, 64 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  bukvoed.addPage( filename.c_str(), roi ); 
#endif

#if 0 // use niblack with 3
  Rect roi = Rect( 2, 61, 238, 85 );
  string filename = string(argv[0]) +"/../../../testdata/temp2013/passport_date.png";
  bukvoed.addPage( filename.c_str(), roi ); 
#endif

#if 1
  //Rect roi = Rect(13, 79, 280, 195 );
  Rect roi = Rect(0,0, 645, 318);
  string filename = "/testdata/dibco11/hw/hw1.png";
  bukvoed.addPage( filename.c_str(), roi ); 
#endif
  //bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_digits_bold.png").c_str() ); 

  //bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_digits2_norm.png").c_str() ); 

  //bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_digits2_bold.png").c_str() ); 


  cout << "addPages ... " << t.msecs() << " milliseconds" << endl;


  bukvoed.makeIndex();
  bukvoed.browse();

  return 0;

}
