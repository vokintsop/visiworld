// bookdata.cpp
#include "precomp.h"

#include "bookdata.h"

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

int BookData::addPage( const char* page_file, Rect roi )
{
  cout << endl << "************************* Processing page " << page_file << endl;
  PageData pd;
  pd.ROI = roi;
  try {
    if (!pd.compute( page_file ))
    {
      cout << "Can't process page " << page_file << endl;
      return -1;
    }
  } catch (...) {
    cout << "Can't process page " << page_file << endl;
    return -1;
  }

  //for (int y=0; y<pd.src.rows; y++) for (int x=0; x<pd.src.cols-45; x++)
  //  if (pd.src[y][x] != pd.src[y][x+45])
  //    cout << "diff at: y=" << y << " x=" << x << endl;

  pages.push_back( pd );

  int iPage = int( pages.size()-1 );

  cout << "\n====== Adding page #" << iPage << " with " << pages[iPage].cc.size() << " components" << endl;
  total_comps_count += int( pages[iPage].cc.size() );
  cout << "Total comps count = " << total_comps_count << endl;


  {
    Ticker t;
    for (int iCC=1; iCC<int(pages[iPage].cc.size()); iCC++) // <<<<<<<<<<< 2 comp -> gpf
    {
      CoverPoint p( iPage, iCC );
      CCData& ccd = pages[iPage].cc[iCC];
      //if (pages[iPage].cc[iCC].sum_f <= 20)
      //  continue;
      //if (pages[iPage].cc[iCC].height() > 60 || pages[iPage].cc[iCC].width() > 60)
      //  continue;
      ccd.isphere = cvnet.insert(p); // запоминаем номер сферы в дереве, к которой привязали компоненту

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


void BookData::makeIndex()
{
  ///// make_index
  /////vector< pair< pair< int, int >, int > > index; // < < sphere_level, count_of_points >, sphere_index >
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
    int pnts = index[i].first.second;
    if ( pnts <= 10)
      continue;

    printf( "\tpnts=%d ", pnts );

    int iNode = index[i].second;
    CoverSphere< CoverPoint > sph = cvnet.getSphere( iNode );
    CoverPoint& cp = sph.center;
    PageData& pd = pages[ cp.iPage ];
    CCData& ccd = pd.cc[cp.iCC];
    Rect rect( ccd.minx, ccd.miny, ccd.maxx - ccd.minx +1, ccd.maxy - ccd.miny +1 ); 

    cv::Mat mmm = pd.src( rect );;
    int a=2;
  }

}

