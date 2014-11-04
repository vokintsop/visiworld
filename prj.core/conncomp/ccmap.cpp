// ccmap.cpp
#include "ocvutils/precomp.h"
//#include "niblack.h"
#include "conncomp/ccmap.h"

#define NO_TICKER_MESSAGES
#include "ocvutils/ticker.h"


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
      if ( row[x] == 255 ) // новая компонента связности
      {
        label_index++;
        cv::floodFill(labels, cv::Point(x,y), label_index, NULL, 0, 0, 4);
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

  t.report("make_labels()");
  return labels;
}

bool CCMap::compute( cv::Mat1b& binarized )
{
  int max_label_index=0;
  vector< Rect > rects;
  labels = make_labels( binarized, max_label_index );

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
  for (int i=1; i< int(cc.size()); i++ )
  {
    CCData& ccd = cc[i];
    ccd.fix();
  }
  return true;
}


void CCMap::draw( cv::Mat& to, Scalar& color, int thickness_not_filtered, int thickness_filtered )
{
  for (int i=1; i< int(cc.size()); i++ ) 
  {
    CCData& ccd = cc[i];

    if ( (ccd.flags & CC_HIDDEN) != 0 )
      continue;

    if ( (ccd.flags & CC_FILTERED) != 0 )
    { // component is filtered
      if (thickness_filtered > 0)
        rectangle( to, Point( ccd.minx-5, ccd.miny-5 ), Point( ccd.maxx+5, ccd.maxy+5 ), color, thickness_filtered );
    }
    else
      rectangle( to, Point( ccd.minx-5, ccd.miny-5 ), Point( ccd.maxx+5, ccd.maxy+5 ), color, thickness_not_filtered );
  }
}
