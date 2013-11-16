#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>

using namespace std;
using namespace cv;


vector< pair< int, cv::Mat > > samples;  // image, class_num

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

int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string data = exe + "/../../../testdata/simeion/simeion.data";
  if (!read_samples(data.c_str()))
    return -1;


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