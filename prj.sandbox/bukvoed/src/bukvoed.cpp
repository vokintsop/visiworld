// bukvoed
#include "precomp.h"

#include "bukvoed.h"

#include "cover_net/cover_net.h"
#include "ocvutils/ticker.h"
#include "pagedata.h"

using namespace std;
using namespace cv;
//////
//////
//////static void open_close_vertical( Mat& in, Mat& ou, bool inverted = false )
//////{
//////  int an = 1;
//////  int element_shape = MORPH_RECT;
//////  Mat element = getStructuringElement(element_shape, Size(1, an*2+1), Point(0,2) );
//////  Mat tmp( in.rows, in.cols, in.type() );
//////  if (inverted)
//////  {
//////    erode( in, tmp, element, Point( 0, 2 ) );
//////    dilate( tmp, ou, element, Point( 0, 2 ) );
//////  }
//////  else
//////  {
//////    dilate( in, tmp, element, Point( 0, 2 ) );
//////    erode( tmp, ou, element, Point( 0, 2 ) );
//////  }
//////
//////  //imshow( "before_open_close", in );
//////  //imshow( "after__open_close", ou );
//////  //waitKey(0);
//////
//////}

///////////////////////////////////////////////////////////////////////////

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


int Bukvoed::browsePages()
{

  int iPage = 0;
  if (pages.size() > 0) do  
  {
    PageData& pd =pages[iPage];
    Mat3b draw_bin; cvtColor( pd.src_binarized, draw_bin, CV_GRAY2BGR ); /// pd.src_binarized.clone();
    Mat3b draw_src; cvtColor( pd.src, draw_src, CV_GRAY2BGR );
    Mat3b draw_dil; cvtColor( pd.src_dilated, draw_dil, CV_GRAY2BGR );

    double ratio = min( 2., 1000./pd.src.rows );
    resize( draw_bin, draw_bin, Size(), ratio, ratio, INTER_AREA );
    resize( draw_src, draw_src, Size(), ratio, ratio, INTER_AREA );
    resize( draw_dil, draw_dil, Size(), ratio, ratio, INTER_AREA );
    for (int iCC =0; iCC<(int)pd.cc.size(); iCC++)
    {
      CCData& ccd = pd.cc[iCC];
      if ( ccd.isphere < 0 )
        continue;
      ///CoverSphere< CoverPoint > sphere = cvnet.getSphere( ccd.isphere );
      const CoverSphere< CoverPoint >& sphere = cvnet.getSphere( ccd.isphere );
      int last_level = cvnet.getCountOfLevels() -1;
      int sphere_level = sphere.level;
      if (sphere_level != last_level)
        continue;
      const CoverPoint& cvpnt = sphere.center;
      const Scalar color = cvpnt.color;
#if 1
      rectangle( draw_bin, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), color, 3  );
      rectangle( draw_src, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), color  );
      rectangle( draw_dil, Point( ccd.minx*ratio, ccd.miny*ratio ), Point( (ccd.maxx+1)*ratio, (ccd.maxy+1)*ratio ), color  );
#endif
    }

    imshow("comps_bin", draw_bin); 
    imshow("comps_src", draw_src); 
    imshow("comps_src_dilated", draw_dil); 
    int key = waitKey(0);
    cout << "Key pressed " << key << endl;
    switch (key)
    {
    case KEY_ESCAPE:
      return 0;
    case KEY_PAGE_DOWN:
      iPage = ((iPage+1) % int( pages.size()) );
      break;
    case KEY_PAGE_UP:
      iPage = ((pages.size()+ iPage-1) % int( pages.size()) );
      break;

    }

  } while (1);
  return 0;
}

int Bukvoed::browseCoverNet()
{
  ///vector< pair< pair< int, int >, int > > Bukvoed::index; // < < sphere_level, count_of_points >, sphere_index >

    //int key = waitKey(0);
    //cout << "Key pressed " << key << endl;
    //switch (key)
    //{
    //case KEY_ESCAPE:
    //  return 0;
    //case KEY_RIGHT_ARROW:
    //  iNode = ((iNode+1) % int( pages.size()) );
    //  break;
    //case KEY_LEFT_ARROW:
    //  iNode = ((pages.size()+ iNode-1) % int( pages.size()) );
    //  break;

    //}
  return 0;
}

