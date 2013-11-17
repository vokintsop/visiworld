// explore mnist dataset
// http://yann.lecun.com/exdb/mnist/

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

int SAMPLE_SIZE = 28;


// ..................... remake below ..................

vector< pair< int, cv::Mat > > samples;  //class_num image, 

bool read_samples( const char* path )
{
  //////////ifstream in( path );

  //////////if (!in.is_open())
  //////////{
  //////////  cout << "Can't open " << path << endl;
  //////////  return false;
  //////////}

  //////////while (in.good())
  //////////{
  //////////  Mat mat( SAMPLE_SIZE, SAMPLE_SIZE, CV_8UC1 );
  //////////  for ( int y=0; y<SAMPLE_SIZE; y++ )
  //////////  {
  //////////    for ( int x=0; x<SAMPLE_SIZE; x++ )
  //////////    {
  //////////      double val=0.5;
  //////////      in >> val;
  //////////      mat.at<uchar>( y, x ) = cvRound( 255*val );
  //////////    }
  //////////  }
  //////////  
  //////////  int chr=-1;
  //////////  for ( int c=0; c<10; c++ )
  //////////  {
  //////////    int cc = -1;
  //////////    in >> cc;
  //////////    if (cc == 1)
  //////////      chr = c;
  //////////    else
  //////////      assert( cc == 0 );
  //////////  }
  //////////  string dummy;
  //////////  getline( in, dummy ); // skip cr-lf

  //////////  if (chr>=0 && chr <=9)
  //////////    samples.push_back( make_pair( chr, mat ) );
  //////////  else
  //////////    break;
  //////////}

  //////////cout << samples.size() << " samples read from " << path << endl;;

  return true;
}

class Metr1
{
public:
  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    double dst=0;
    Mat m1 = samples[i1].second;
    Mat m2 = samples[i2].second;
    for ( int y=0; y<SAMPLE_SIZE; y++ )
    {
      for ( int x=0; x<SAMPLE_SIZE; x++ )
      {
        if (m1.at<uchar>( y, x ) != m2.at<uchar>( y, x ) )
          dst += 1;
      }
    }
    return dst;
  }
};


void explore_cover_tree()
{

  Metr1 ruler;
  CoverTree< int, Metr1 > tree( &ruler, 1000, 1 );
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

  explore_cover_tree();


  int key=-1;
  for ( int frame =0; key != 27 && frame < int( samples.size() ); )
  {
    Mat matx;
    resize( samples[frame].second, matx, Size(), 16., 16., INTER_AREA ); // расшир€ем в 16 раз дл€ удобного просмотра

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