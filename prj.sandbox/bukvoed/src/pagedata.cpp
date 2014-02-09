// pagedata.cpp
#include "precomp.h"

#include "niblack.h"
#include "pagedata.h"
#include "ticker.h"


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



static Mat make_labels( // на выходе Mat, в котором значения пикселей: 0 -- компонента связности фона
                 // 1..max_label_index -- соответствуют номерам компонент связности
                const cv::Mat &bw, // < threshold сигнал, >= threshold фон
                int& max_label_index, // максимальный индекс компонент связности; 
                //vector< cv::Rect >& rects,
                int threshold = 128  // пиксели >= threshold считаются фоновыми (белыми)
                ) 
{
  Ticker t;  
  Mat labels;
  bw.convertTo(labels, CV_32SC1);
  int label_index = 255; 
  //rects.clear();
  for(int y=0; y < labels.rows; y++)
  {
    int *row = (int*)labels.ptr(y);
    for(int x=0; x < labels.cols; x++) 
    {
      if( row[x] >= threshold && row[x] <= 255 ) // белый фон
          continue;
      if ( row[x] < threshold ) // черная не размеченная буква -- новая компонента связности
      {
        label_index++;
        cv::floodFill(labels, cv::Point(x,y), label_index, NULL, 0, 0, 4);
        //cv::Rect rect;
        //cv::floodFill(labels, cv::Point(x,y), label_index, &rect, 0, 0, 4);
        //rects.push_back( rect );
        //assert( label_index + 255 == rects.size() );
      }
    }
  }
  for(int y=0; y < labels.rows; y++)
  {
    int *row = (int*)labels.ptr(y);
    for(int x=0; x < labels.cols; x++) 
    {
      if( row[x] > 255 )
        row[x] = row[x]-255;
      else
        row[x] = 0;
    }
  }
  max_label_index = label_index - 255;

  cout << "make_labels() ... " << t.msecs() << " milliseconds" << endl;
  return labels;
}




bool PageData::compute( const char* filename )
{
  source_filename = filename;
#if 0
  {
    Mat src_rgb = imread( filename ); 
    if (ROI.area() > 0)
      src_rgb = src_rgb( ROI ); 
    imshow( "Original image", src_rgb );
  }
#endif
  src = imread( filename, IMREAD_GRAYSCALE );
////////////// preprocess image
  if (ROI.area() > 0)
    src = src( ROI ); 



  // filter
  //open_close_vertical( src, src );
  //blur( src, src, Size(3,3) );
  //equalizeHist( src, src );
////////////////////////////////


  //src_binarized();
  //double thresh = threshold( src, src_binarized, 128., 255., THRESH_BINARY | CV_THRESH_OTSU );
  
  //int res = niblack( src, src_binarized, 5, 0.2 );
  
  int res = niblack( src, src_binarized );

#define DILATE_AFTER_BINARIZATION
#ifdef DILATE_AFTER_BINARIZATION
  dilate1( src_binarized, src_dilated, true );
#else
  dilate1( src, src_dilated, true );
#endif

  string outbin = filename; outbin += ".png";
  imwrite( outbin, src_binarized );

  
#if 0
  imshow( "src", src );
  imshow( "src_binarized", src_binarized );
  waitKey(0);
#endif

  int max_label_index=0;
  vector< Rect > rects;
  labels = make_labels( src_binarized, max_label_index );

  cc.clear(); 
  cc.resize( max_label_index+1 );
  for(int y=0; y < labels.rows; y++)
  {
    int *row = (int*)labels.ptr(y);
    for(int x=0; x < labels.cols; x++) 
    {
      if( row[x] == 0  )
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
