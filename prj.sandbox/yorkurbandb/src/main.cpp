// explore yorkurbandb dataset
//
// dataset was presented in paper
// P. Denis, J. Elder, and F. Estrada, 
// УEfficient Edge-Based Methods for Estimating Manhattan Frames in Urban ImageryФ, 
// European Conference on Computer Vision, part II, pp. 197-210, 2008
// downloaded from
// http://http://elderlab.yorku.ca/YorkUrbanDB/

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>

#include "cover_net.h"
#include "imagerecord.h"

using namespace std;
using namespace cv;





int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  //string folder = exe + "/../../../testdata/yorkurbandb";
  string folder = "//testdata/yorkurbandb";
  vector< ImageRecord > image_records; // данные по картинкам
  if (!read_image_records( folder, image_records ) || image_records.size() < 1 )
    return -1;

  for (int i=0; i<int( image_records.size() ); i++)
    image_records[i].explore();

  make_report( image_records );

  return 0;
}

bool read_image_records( string& root, vector< ImageRecord >& image_records )
{
  // todo: fill image_records[] inputs
  return true;
}

void ImageRecord::explore()
{

  ////Metr1 ruler;  CoverTree< int, Metr1 > tree( &ruler, 1000, 1 );
  //Metr2 ruler;  CoverTree< int, Metr2 > tree( &ruler, 1000, 1 );

  //for (int i=0; i< int( samples.size() ); i++)
  //  tree.insert( i );

  //tree.reportStatistics( 0, 3 ); 

}


//
//class Metr1 // .........
//{
//public:
//  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
//  {
//    double dst=0;
//    Mat m1 = samples[i1].second;
//    Mat m2 = samples[i2].second;
//    for ( int y=0; y<16; y++ )
//    {
//      for ( int x=0; x<16; x++ )
//      {
//        if (m1.at<uchar>( y, x ) != m2.at<uchar>( y, x ) )
//          dst += 1;
//      }
//    }
//    return dst;
//  }
//};