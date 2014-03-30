// explore yorkurbandb dataset
//
// dataset was presented in paper
// P. Denis, J. Elder, and F. Estrada, 
// “Efficient Edge-Based Methods for Estimating Manhattan Frames in Urban Imagery”, 
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

using namespace std;
using namespace cv;

bool read_samples( const char* path )
{
  ifstream in( path );

  if (!in.is_open())
  {
    cout << "Can't open " << path << endl;
    return false;
  }

// todo astdcall

/*
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
*/
  cout << samples.size() << " samples read from " << path << endl;;

  return true;
}

class Metr1 // .........
{
public:
  double computeDistance( const int& i1,  const int& i2 )  // èíäåêñû ê samples[]
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

void explore_yourkurbandb()
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
  //string data = exe + "/../../../testdata/yourkurbandb";
  string data = "d:/testdata/yourkurbandb";
  if (!read_samples(data.c_str()))
    return -1;

  explore_yourkurbandb();

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