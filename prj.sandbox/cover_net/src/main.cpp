#include <conio.h>

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
#include "ticker.h"

using namespace std;
using namespace cv;

int SAMPLE_HEIGHT = 0;
int SAMPLE_WIDTH = 0;

// train:
vector< pair< int, cv::Mat > > trn_samples;  // image, class_num
vector< pair< int, cv::Mat > > trn_samples_dilated;  // image, class_num

// test:
vector< pair< int, cv::Mat > > tst_samples;  // image, class_num
vector< pair< int, cv::Mat > > tst_samples_dilated;  // image, class_num

//only for intel
inline unsigned long ntohl(unsigned long n) {
  return ((n & 0xFF) << 24) | ((n & 0xFF00) << 8) | ((n & 0xFF0000) >> 8) | ((n & 0xFF000000) >> 24);
}

bool _read_samples( string data, // images
                    string data2, // labels
                    vector< pair< int, cv::Mat > >& samples // result vector
                    );

bool read_samples( string mnist_folder )
{
	string trndata = mnist_folder + "/train-images.idx3-ubyte";
	string trndata1 = mnist_folder + "/train-labels.idx1-ubyte";
  bool ok1 = _read_samples( trndata, trndata1, trn_samples );

	string tstdata = mnist_folder + "/t10k-images.idx3-ubyte";
	string tstdata1 = mnist_folder + "/t10k-labels.idx1-ubyte";
  bool ok2 = _read_samples( tstdata, tstdata1, tst_samples );
  
  return ok1 && ok2;
}

bool _read_samples( string patterns, // images
                    string labels, // labels
                    vector< pair< int, cv::Mat > >& samples // result vector
                    )
{	
  cout << "Reading patterns from " << patterns << endl;
  cout << "using labels from " << labels << endl;
	ifstream in(patterns.c_str(), ios::binary|ios::in);
	ifstream in1(labels.c_str(), ios::binary|ios::in);
	
	if (!in.is_open())
	{
		cout << "Can't read patterns from " << patterns << endl;
		return false;
	}
	if (!in1.is_open())
	{
		cout << "Can't read labels from " << labels << endl;
		return false;
	}
 	
	int magic_number;
    in.read(reinterpret_cast<char*>(&magic_number), sizeof(magic_number));
	magic_number = ntohl(magic_number);

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
    if (i%1000 == 0)
      cout << ".";
		Mat1b mymat(rows, cols);
		unsigned char label;
		in1.read(reinterpret_cast<char*>(&label), sizeof(label));
		for (int i1 = 0; i1 < cols; ++i1)
		{
			for (int i2 = 0; i2 < rows; ++i2)
			{
				unsigned char pixel;
				in.read(reinterpret_cast<char*>(&pixel), sizeof(pixel));
				mymat(i1, i2) = pixel;
			}
		}
		samples.push_back(make_pair(label, mymat));
	}

  cout << endl << samples.size() << " samples read from " << patterns << endl;

  return true;
}

void _dilate_samples(
  vector< pair< int, cv::Mat > >& samples, // result vector
  vector< pair< int, cv::Mat > >& samples_dilated // result vector
                     )
{
  samples_dilated.resize(0);
  int an = 1;
  int element_shape = MORPH_CROSS; // MORPH_RECT;
  Mat element = getStructuringElement(element_shape, Size(an*2+1, an*2+1), Point(an, an) );
  for (int i=0; i< int( samples.size() ); i++)
  {
    
    Mat m( SAMPLE_HEIGHT, SAMPLE_WIDTH, CV_8UC1 );
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

void dilate_samples()
{
  _dilate_samples( trn_samples, trn_samples_dilated );
  _dilate_samples( tst_samples, tst_samples_dilated );
}

class Metr1
{
public:
  vector< pair< int, cv::Mat > > *samples1, *samples2;
  Metr1() : samples1(0),samples2(0) {};

  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    double dst=0;
    Mat m1 = (*samples1)[i1].second;
    Mat m2 = (*samples2)[i2].second;
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

class Metr2 // A intersect dilated(B) + B intersect dilated(A)
{
public:
  vector< pair< int, cv::Mat > > *samples1, *samples2;
  vector< pair< int, cv::Mat > > *samples1_dilated, *samples2_dilated;

  Metr2():  samples1(0), samples2(0), samples1_dilated(0), samples2_dilated(0){}

  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    double dst=0;
    Mat m1 = (*samples1)[i1].second;    Mat m1ex = (*samples1_dilated)[i1].second; 
    Mat m2 = (*samples2)[i2].second;    Mat m2ex = (*samples2_dilated)[i2].second; 
    for ( int y=0; y<16; y++ )
    {
      for ( int x=0; x<16; x++ )
      {
        //if (m1.at<uchar>( y, x ) == 255 && m2ex.at<uchar>( y, x ) != 255 )
        //  dst += 1;
        //if (m2.at<uchar>( y, x ) == 255 && m1ex.at<uchar>( y, x ) != 255 )
        //  dst += 1;
#if 1 // -------- good --------
        if (m1.at<uchar>( y, x ) == 255 && m2ex.at<uchar>( y, x ) != 255 )
          dst += abs( m1.at<uchar>( y, x ) - m2ex.at<uchar>( y, x ) );
        if (m2.at<uchar>( y, x ) == 255 && m1ex.at<uchar>( y, x ) != 255 )
          dst += abs( m2.at<uchar>( y, x ) - m1ex.at<uchar>( y, x ) );
#endif
#if 0
        if (m1.at<uchar>( y, x ) > m2ex.at<uchar>( y, x ))
          dst += abs( m1.at<uchar>( y, x ) - m2ex.at<uchar>( y, x ) );
        if (m2.at<uchar>( y, x ) > m1ex.at<uchar>( y, x ))
          dst += abs( m2.at<uchar>( y, x ) - m1ex.at<uchar>( y, x ) );
#endif
      }
    }
    return dst;
  }
};

void explore_cover_tree()
{
  bool test_hamming= true;
  bool test_smart = false;

  Metr1 ruler1;
  Metr2 ruler2;
  for (int chr =0; chr<=10; chr++)
  {
    if (chr < 10)
      cout << "------ cvnet report of class " << chr << endl;
    else
      cout << "------ cvnet report of classes 0..9 union "<< endl;

    
    ruler1.samples1 = &trn_samples;
    ruler1.samples2 = &trn_samples; // both from trn_samples
    CoverNet< int, Metr1 > cvnet1( &ruler1, SAMPLE_HEIGHT*SAMPLE_WIDTH*256, 1 );
    // both from trn_samples
    ruler2.samples1 = &trn_samples;    ruler2.samples1_dilated = &trn_samples_dilated;
    ruler2.samples2 = &trn_samples;    ruler2.samples2_dilated = &trn_samples_dilated;

    CoverNet< int, Metr2 > cvnet2( &ruler2, SAMPLE_HEIGHT*SAMPLE_WIDTH*256, 1 );

    if (test_hamming)
    {
      Ticker t;
      for (int i=0; i< int( trn_samples.size() ); i++)
      {
        if (chr == 10 || trn_samples[i].first == chr)
        {
          cvnet1.insert( i );
        }
      }
      double ms = t.msecs();
      cout << "\nHamming metrics (simple L1):" << endl;
      cvnet1.reportStatistics( 0, 3 ); 
      cout << "Build time = " << ms/1000 << " seconds" << endl;
    }

    if (test_smart)
    {
      Ticker t;
      for (int i=0; i< int( trn_samples.size() ); i++)
      {
        if (chr == 10 || trn_samples[i].first == chr)
        {
          cvnet2.insert( i );
        }
      }
      double ms = t.msecs();
      cout << "\nA vs dilate(B) + B vs dilate(A) metrics (L1):" << endl;
      cvnet2.reportStatistics( 0, 3 ); 
      cout << "Build time = " << ms/1000 << " seconds" << endl;
    }

    // test recognition
    if (chr == 10)
    {
      ruler1.samples1 = &trn_samples;
      ruler1.samples2 = &tst_samples;

	  ruler2.samples1 = &trn_samples;
      ruler2.samples2 = &tst_samples;

	  ruler2.samples1_dilated = &trn_samples_dilated;
	  ruler2.samples2_dilated = &tst_samples_dilated;

      double max_hit_distance = -1; 
      double min_miss_distance = std::numeric_limits<double>::max(); 

      //ruler2.samples1 = &tst_samples;    ruler2.samples1_dilated = &tst_samples_dilated;
      //ruler2.samples2 = &trn_samples;    ruler2.samples2_dilated = &trn_samples_dilated;
      int hit=0; int miss=0;

	  // 

	  cout << "begin test " << endl;
      for (int i_tst=0; i_tst<int(tst_samples.size()); i_tst++)
      {
        double distance = SAMPLE_HEIGHT*SAMPLE_WIDTH*256; // а могли бы и отсечение указать?
        int i_trn = cvnet1.findNearestPoint(i_tst, distance);// надо добавить поиск ближайшей точки от какого-то образца i_tst до trn_samples 
/// my check

		imshow("test_mat", tst_samples[i_tst].second);
		imshow("best_vertex", trn_samples[i_trn].second);
		cerr << "Result:  tst_value = " << tst_samples[i_tst].first << " trn_value = " << trn_samples[i_trn].first << " dist = " << distance << endl;
		cvWaitKey(500);

/////
		if (tst_samples[i_tst].first == trn_samples[i_trn].first)
        {
          max_hit_distance = max( max_hit_distance, distance );
          hit++;
        }
        else
        {
          min_miss_distance = min( min_miss_distance, distance );
          miss++;
        }
      }
      cout << "Hits = " << hit << " \tMisses = " << miss << endl;
      cout << "Max hit dist = " << max_hit_distance << " Min miss dist = " << min_miss_distance << endl;
    }
  }
#if 0
  cout << "press any key to continue" << endl;
  _getch();
#endif

}


int main( int argc, char* argv[] )
{
  string exe = argv[0];
  string mnist_folder = exe + "/../../../testdata/mnist";
	read_samples(mnist_folder);

  dilate_samples();

  explore_cover_tree();

  cout << "\n\n ======= Press any key to finish... ========" << endl;
  _getch();
  //return 1;

  int key=-1;
  for ( int frame =0; key != 27 && frame < int( tst_samples.size() ); )
  {
    Mat matx;
    resize( tst_samples[frame].second, matx, Size(), 16., 16., INTER_AREA ); // расширяем в 16 раз для удобного просмотра

    imshow( "sample", matx );
	
    key = waitKey(50);
    switch (key)
    {
    case 27: break;
    default:
      frame = (frame+1) % tst_samples.size();
    }
  }


	return 0;
}