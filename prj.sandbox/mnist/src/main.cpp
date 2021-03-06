// explore mnist dataset
// http://yann.lecun.com/exdb/mnist/

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> 


#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cassert>


#include "cover_tree.h"
//#include <Winsock2.h>

using namespace std;
using namespace cv;

int SAMPLE_HEIGHT = 0;
int SAMPLE_WIDTH = 0;


// ..................... remake below ..................

vector< pair< int, cv::Mat > > samples;  //class_num image, 

inline unsigned long ntohl(unsigned long n) {
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}
inline unsigned long htonl(unsigned long n) {
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

bool read_samples( string mnist_folder )
{

	string data = mnist_folder + "/train-images.idx3-ubyte";
	string data1 = mnist_folder + "/train-labels.idx1-ubyte";
	
	ifstream in(data.c_str(), ios::binary|ios::in);
	ifstream in1(data1.c_str(), ios::binary|ios::in);
	
	if (!in.is_open() || !in.is_open())
	{
		cout << "no file" << endl;
		return false;
	} 
 	
	int magic_number;
  in.read(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
	magic_number = ntohl(magic_number);

  cout << magic_number << endl;

	int magic_number1;
    in1.read(reinterpret_cast<char*>(&magic_number1), sizeof(magic_number1));
	magic_number1 = ntohl(magic_number1);

	int img_num;
    in.read(reinterpret_cast<char*>(&img_num), sizeof(img_num));
	img_num = ntohl(img_num);

	int img_num1;
    in1.read(reinterpret_cast<char*>(&img_num1), sizeof(img_num1));
	img_num1 = ntohl(img_num1);

	int rows;
	in.read(reinterpret_cast<char*>(&rows), sizeof(rows));
	rows = ntohl(rows);
  SAMPLE_HEIGHT = rows;
	
	int cols;
	in.read(reinterpret_cast<char*>(&cols), sizeof(cols));
	cols = ntohl(cols);
  SAMPLE_WIDTH = cols;

	for (int i = 0; i < img_num; ++i)
	{
		Mat1b mymat(rows, cols);
		unsigned char label;
		in1.read(reinterpret_cast<char*>(&label), sizeof(label));
    //cout << label << endl;
		for (int i1 = 0; i1 < rows; ++i1)
		{
			for (int i2 = 0; i2 < cols; ++i2)
			{
				unsigned char pixel;
				in.read(reinterpret_cast<char*>(&pixel), sizeof(pixel));
				mymat(i1, i2) = pixel;
			}
     
		} 
		samples.push_back(make_pair(label, mymat));
	}

  cout << samples.size() << " samples read from " << data << endl;;

  return true;
}

class Metr1
{
public:
  double computeDistance( const int& i1,  const int& i2 )  // ������� � samples[]
  {
    double dst=0;
    Mat m1 = samples[i1].second;
    Mat m2 = samples[i2].second;
    for ( int y=0; y< SAMPLE_HEIGHT; y++ )
    {
      for ( int x=0; x< SAMPLE_WIDTH; x++ )
      {
        //if (m1.at<uchar>( y, x ) != m2.at<uchar>( y, x ) )
        //  dst += 1;
        int d1 = m1.at<uchar>( y, x );
        int d2 = m2.at<uchar>( y, x );
        dst += abs(d1-d2);
      }
    }
    return dst;
  }
};


void explore_cover_tree()
{

  Metr1 ruler;
  CoverTree< int, Metr1 > tree( &ruler, 256*256, 1 );
  for (int i=0; i< int( samples.size() ); i+=16)
    tree.insert( i );

  tree.reportStatistics( 0, 3 ); 

}

int main( int argc, char* argv[] )
{
  string exe = argv[0];
  string mnist_folder = exe + "/../../../testdata/mnist";
	read_samples(mnist_folder);

  explore_cover_tree();


  int key=-1;
  for ( int frame =0; key != 27 && frame < int( samples.size() ); )
  {
    Mat matx;
    resize( samples[frame].second, matx, Size(), 5., 5., INTER_AREA ); // ��������� � 16 ��� ��� �������� ���������

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