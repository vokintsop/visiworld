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


vector< pair< cv::Mat, int > > samples;

int main( int argc, char* argv[] )
{
  string exe  = argv[0];
  string data = exe + "/../../../testdata/simeion/simeion.data";
  //freopen( data.c_str(), "r", stdin );
  ifstream in( data.c_str() );

  if (!in.is_open())
  {
    cout << "Can't open " << data << endl;
    return -1;
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
      samples.push_back( make_pair( mat, chr ) );
    else
      break;

    Mat matx;
    resize( mat, matx, Size(), 8., 8. );

    imshow( "letter", matx );
    waitKey(1);
  }


  /*



  
  for ( int frame =0, int key=0; key != 27 && frame < lines.size(); frame = frame ++)
  {
    cv::Mat mat = line2mat( lines[i] );
    imshow( "letter", mat );
    key = waitKey(0);
  }
  */
	return 0;
}