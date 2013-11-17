// explore semeion dataset
// http://archive.ics.uci.edu/ml/machine-learning-databases/semeion/

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>

#include "cover_tree.h"

using namespace std;
using namespace cv;




vector< pair< int, cv::Mat > > samples;  // image, class_num
vector< pair< int, cv::Mat > > samples_dilated;  // image, class_num

bool read_samples( const char* path )
{
  ifstream in( path );

  if (!in.is_open())
  {
    cout << "Can't open " << path << endl;
    return false;
  }

  while (in.good())
  {
    Mat mat( 16, 16, CV_8UC1 );
    for ( int y=0; y<16; y++ )
    {
      for ( int x=0; x<16; x++ )
      {
        double val=0.5;
        in >> val;
        mat.at<uchar>( y, x ) = cvRound( 255*val );
      }
    }
    
    int chr=-1;
    for ( int c=0; c<10; c++ )
    {
      int cc = -1;
      in >> cc;
      if (cc == 1)
        chr = c;
      else
        assert( cc == 0 );
    }
    string dummy;
    getline( in, dummy ); // skip cr-lf

    if (chr>=0 && chr <=9)
      samples.push_back( make_pair( chr, mat ) );
    else
      break;
  }

  cout << samples.size() << " samples read from " << path << endl;;

  return true;
}

void dilate_samples()
{
  samples_dilated.resize(0);
  int an = 1;
  int element_shape = MORPH_CROSS; // MORPH_RECT;
  Mat element = getStructuringElement(element_shape, Size(an*2+1, an*2+1), Point(an, an) );
  for (int i=0; i< int( samples.size() ); i++)
  {
    Mat m( 16, 16, CV_8UC1 );
    dilate( samples[i].second, m, element, Point( an, an ) );
#if 0
    Mat mx1; resize( m, mx1, Size(), 16, 16, INTER_AREA  );
    Mat mx2; resize( samples[i].second, mx2, Size(), 16, 16, INTER_AREA  );
    imshow("initial", mx2 );
    imshow("dilated", mx1 );
    waitKey(0);
#endif
    samples_dilated.push_back( make_pair( samples[i].first, m ) );
  }
}

class Metr1 // simple hamming
{
public:
  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    double dst=0;
    Mat m1 = samples[i1].second;
    Mat m2 = samples[i2].second;
    for ( int y=0; y<16; y++ )
    {
      for ( int x=0; x<16; x++ )
      {
        if (m1.at<uchar>( y, x ) != m2.at<uchar>( y, x ) )
          dst += 1;
      }
    }
    return dst;
  }
};


class Metr2 // A intersect dilated(B) + B intersect dilated(A)
{
public:
  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    double dst=0;
    Mat m1 = samples[i1].second;    Mat m1ex = samples_dilated[i1].second; 
    Mat m2 = samples[i2].second;    Mat m2ex = samples_dilated[i2].second; 
    for ( int y=0; y<16; y++ )
    {
      for ( int x=0; x<16; x++ )
      {
        if (m1.at<uchar>( y, x ) == 255 && m2ex.at<uchar>( y, x ) != 255 )
          dst += 1;
        if (m2.at<uchar>( y, x ) == 255 && m1ex.at<uchar>( y, x ) != 255 )
          dst += 1;
      }
    }
    return dst;
  }
};

void explore_cover_tree()
{

  //Metr1 ruler;  CoverTree< int, Metr1 > tree( &ruler, 1000, 1 );
  Metr2 ruler;  CoverTree< int, Metr2 > tree( &ruler, 1000, 1 );

  for (int i=0; i< int( samples.size() ); i++)
    tree.insert( i );

  tree.reportStatistics( 0, 3 ); 

}

int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string data = exe + "/../../../testdata/semeion/semeion.data";
  if (!read_samples(data.c_str()))
    return -1;

  dilate_samples();

  explore_cover_tree();

  int key=-1;
  for ( int frame =0; key != 27 && frame < int( samples.size() ); )
  {
    Mat matx;
    resize( samples[frame].second, matx, Size(), 16., 16. );

    imshow( "sample", matx );
    key = waitKey(50);
    switch (key)
    {
    case 27: break;
    default:
      frame = (frame+1) % samples.size();
    }
  }


	return 0;
}