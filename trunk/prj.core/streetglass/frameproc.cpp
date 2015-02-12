#include "ocvutils/precomp.h"
#include "streetglass/frameproc.h"
#include "detect/detect_segment.h"
#include "binarize/niblack.h"
#include "detect/fht.h"

void resize4max( Mat1b& inp, Mat1b& out )
{
  int w4 = inp.cols/4;
  int h4 = inp.rows/4;
  
  out.create( h4, w4 );
  for (int y=0; y<h4; y++) 
  {
    for (int x=0; x<w4; x++)
    {
      int xx = x*4;    int yy = y*4;
      int p0 = max( max( inp[yy+0][xx+0], inp[yy+0][xx+1] ), max( inp[yy+0][xx+2], inp[yy+0][xx+3] ) );
      int p1 = max( max( inp[yy+1][xx+0], inp[yy+1][xx+1] ), max( inp[yy+1][xx+2], inp[yy+1][xx+3] ) );
      int p2 = max( max( inp[yy+2][xx+0], inp[yy+2][xx+1] ), max( inp[yy+2][xx+2], inp[yy+2][xx+3] ) );
      int p3 = max( max( inp[yy+3][xx+0], inp[yy+3][xx+1] ), max( inp[yy+3][xx+2], inp[yy+3][xx+3] ) );
      out[y][x] = max( max( p0, p1 ), max( p2, p3 ) );
    }
  }
}

void resize2max( Mat1b& inp, Mat1b& out )
{
  int w2 = inp.cols/2;
  int h2 = inp.rows/2;
  
  out.create( h2, w2 );
  for (int y=0; y<h2; y++) 
  {
    for (int x=0; x<w2; x++)
    {
      int xx = x*2;    int yy = y*2;
      int p0 = max( inp[yy+0][xx+0], inp[yy+0][xx+1] );
      int p1 = max( inp[yy+1][xx+0], inp[yy+1][xx+1] );
      out[y][x] = max( p0, p1 );
    }
  }
}

Mat1b combine( Mat1b& r, Mat1b& g, Mat1b& b )
{
  assert( r.rows == g.rows );
  assert( r.rows == b.rows );
  assert( r.cols == g.cols );
  assert( r.cols == b.cols );

  Mat1b u( r.rows, r.cols );
  for (int y=0; y<u.rows; y++)
    for (int x=0; x<u.cols; x++)
      u[y][x] = saturate_cast<uchar>( 2*r[y][x] - g[y][x] - b[y][x] );
  return u;
}



static void open_vertical( Mat& in, Mat& ou, int anx, int any, bool inverted = false )
{
  int element_shape = MORPH_RECT; // MORPH_RECT;
  Mat element = getStructuringElement(element_shape, Size(anx*2+1, any*2+1), Point(anx, any) );
  if (!inverted)
  {
    dilate( in, ou, element, Point( anx, any ) );
    erode( in, ou, element, Point( anx, any ) );
  }
  else
  {
    erode( in, ou, element, Point( anx, any ) );
    dilate( in, ou, element, Point( anx, any ) );
  }

  //imshow( "before_dilation", in );
  //imshow( "after_dilation", ou );
  //waitKey(0);

}


bool FrameProc::process( cv::Mat& input_bgr720, int scheme )// подготовка основных рабочих битмапов
{

  //if (input_bgr720.empty() || input_bgr720.rows != 720)
  //  return false;


  //bgr720 = input_bgr720.clone();
  bgr720 = input_bgr720; //.clone();
  fht = Mat1i(0,0);
  if (detailed_visualization)
    imwrite("/temp/markup/bgr_image.png", bgr720);

  


  //Mat1b ii; cvtColor( bgr720, ii, COLOR_BGR2GRAY );
  //imshow( "intens", ii );


  //blur( bgr720, bgr720, Size(3,3) ); 

  //vector< Stick > vsticks;
  //if (scheme & FP_STICKS)
  //{
  //  computeVerticalSticsAsMat1b( bgr720, vsticks1b );
  //  imshow("vsticks1b", vsticks1b);
  //}

  if (!compute_binmask( scheme ))
    return false;
/*
  if (!redmask.empty())
    imshow("redmask", redmask); 
  if (!gremask.empty())
    imshow("gremask", gremask);
  if (!blumask.empty())
    imshow("blumask", blumask); 
  //waitKey(1); // redraw
*/
  if (!compute_cc())
    return false;
  return true;
}

bool FrameProc::draw( cv::Mat& display, const string& objType )// визуализация
{
  static bool binmaskwindow = false;
  cv::Mat1b * pmask=NULL;
  if      (objType.find( "AFO_Red" ) == 0) {
    Scalar red( 128, 128, 0 );
    redcc.draw( display, red, 2, 1 ), pmask = &redmask;
  } else if (objType.find( "AFO_Gre" ) == 0) {
    Scalar green( 128, 0, 128 );
    grecc.draw( display, green, 2, 1 ), pmask = &gremask;
  } else  if (objType.find( "AFO_Blu" ) == 0) {
    Scalar blue( 0, 128, 128 );
    blucc.draw( display, blue, 2, 1 ), pmask = &blumask;
  }

  if (detailed_visualization)


  if (detailed_visualization && pmask != NULL) 
  {
      cv::namedWindow( "FrameProc::binmask", cv::WINDOW_NORMAL );
      imshow("FrameProc::binmask", *pmask), imwrite("/temp/markup/binmask.png", *pmask), binmaskwindow=true;
  }
  else if (binmaskwindow)
    cv::destroyWindow("FrameProc::binmask"), binmaskwindow=false;


  return true;
}

bool FrameProc::compute_binmask( int scheme )
{
  vector<Mat1b> channels;
#if 0 /// small mask
  resize( bgr720, bgr360, Size(0,0), 0.5, 0.5, INTER_AREA );
  split(bgr360, channels);
#else
  split(bgr720, channels);
#endif

#ifdef __ANDROID__
  Mat1b b2 = channels[2];
  Mat1b g2 = channels[1];
  Mat1b r2 = channels[0];
#else
  Mat1b b2 = channels[0];
  Mat1b g2 = channels[1];
  Mat1b r2 = channels[2];
#endif

  if (scheme & FP_REDCC)
  {
    Mat1b rg2 = r2 - g2;
    Mat1b rb2 = r2 - b2;
    redmask = rg2 + rb2;
    //imshow("redmask_before_threshold", redmask);

    cv::imwrite( "redmask_input.jpg", redmask );

    cv::Mat1b tempRedmask = redmask.clone();
    cv::GaussianBlur( tempRedmask, tempRedmask, cv::Size(101, 101), 100, 100 );
    cv::imwrite( "GaussianBlur.jpg", tempRedmask );
    tempRedmask = redmask - tempRedmask;

    double min_pix_value = 0, max_pix_value = 0;
    cv::minMaxLoc( tempRedmask, &min_pix_value, &max_pix_value );
    tempRedmask.convertTo( tempRedmask, -1, 255.0 / (max_pix_value - min_pix_value), 
                           -255.0 * min_pix_value / (max_pix_value - min_pix_value) );
    cv::imwrite( "tempRedmask0.jpg", tempRedmask );

//     int i = 1;
//     cv::Mat img_temp_tempRedmask;
//     cv::bilateralFilter( tempRedmask, img_temp_tempRedmask, i, 2*i, i/2 );
//     tempRedmask = img_temp_tempRedmask;
//     cv::imwrite( "after_bilateralFilter.jpg", tempRedmask );

    /*redmask = tempRedmask;*/
    // niblack_standart( tempRedmask, redmask, 15, 0.5, 50 * sensitivity, true );
    niblack( tempRedmask, redmask, 10 * sensitivity, 15, 0.5, true );

//     double thresh = threshold( redmask, redmask, 64 * sensitivity, 255., THRESH_BINARY /*| CV_THRESH_OTSU*/ );

    cv::imwrite( "redmask.jpg", redmask );

    //imshow("redmask_before_morf", redmask);
    //open_vertical( redmask, redmask, 1, 2 );
  }
  if (scheme & FP_BLUCC)
  {
    Mat1b bg2 = b2 - g2;
    Mat1b br2 = b2 - r2;
    blumask = bg2 + br2;
    double thresh = threshold( blumask, blumask, 128 * sensitivity, 255., THRESH_BINARY /*| CV_THRESH_OTSU*/ );
    //open_vertical( blumask, blumask, 5, 5 );
  }
  if (scheme & FP_GRECC)
  {
#if 0
    Mat1b gb2 = g2 - b2;
    Mat1b gr2 = g2 - r2;
    gremask = gb2 + gr2;

    double thresh = threshold( gremask, gremask, 96, 255., THRESH_BINARY  /*| CV_THRESH_OTSU*/  );
    open_vertical( gremask, gremask, 1, 2 );
#endif
    gremask = g2*0.6 + b2*0.4;
    //imshow("gremask",gremask);

    //open_vertical( gremask, gremask, 4, 4 );
    double thresh = threshold( gremask, gremask, 128 * sensitivity, 255., THRESH_BINARY  /*| CV_THRESH_OTSU*/  );
    //imshow("gremask_bin",gremask);

  }
  return true;
}

bool FrameProc::compute_cc()
{
  // на входе cv::Mat1b redmask; // бинаризованная картинка красных огоньков
  // на выходе CCMap redcc; // красные компоненты связности
  if (!redmask.empty() && !redcc.compute( redmask ))
    return false;
  // filter
  for (int i=1; i<int(redcc.cc.size()); i++)
  {
    //if (redcc.cc[i].filter_by_area( 25, 1000 ))
    //  continue;
    redcc.cc[i].filter_by_size( 3, 70, 4, 80 );
    redcc.cc[i].filter_by_ratio( 0.5, 1.3 );
  }

  if (!gremask.empty() && !grecc.compute( gremask ))
    return false;
  // filter
  for (int i=1; i<int(grecc.cc.size()); i++)
  {
    //if (grecc.cc[i].filter_by_area( 25, 1000 ))
    //  continue;
    grecc.cc[i].filter_by_size( 3, 70, 4, 80 );
    grecc.cc[i].filter_by_ratio( 0.25, 1.3 );
  }

  if (!blumask.empty() && !blucc.compute( blumask ))
    return false;
  // filter
  for (int i=1; i<int(blucc.cc.size()); i++)
  {
    //if (blucc.cc[i].filter_by_area( 25, 1000 ))
    //  continue;
    if (blucc.cc[i].filter_by_size( 3, 60, 3, 60 ))
      continue;
  }
  return true;
}


bool FrameProc::detect_segment(  // детектирует наилучший сегмент в области указанной точки или отрезка
  std::vector< cv::Point >& pts // in-out, одна или несколько точек, в окрестности которых надо искать отрезок
  )
{
  if (fht.size() == Size(0, 0))
    count_fht(kFHT, bgr720, fht);
  return DetectSegment( fht, pts, kFHT );
}




