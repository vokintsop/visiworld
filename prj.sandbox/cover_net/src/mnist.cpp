// test cover_net on mnist dataset

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


#include "cover_net2.h"
#include "ticker.h"
#include "test.h"

using namespace std;
using namespace cv;

int SAMPLE_HEIGHT = 0;
int SAMPLE_WIDTH = 0;

//#define simple_tester

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

static bool _read_samples( string data, // images
                    string data2, // labels
                    vector< pair< int, cv::Mat > >& samples // result vector
                    );

static bool read_samples( string mnist_folder )
{
	string trndata = mnist_folder + "/train-images.idx3-ubyte";
	string trndata1 = mnist_folder + "/train-labels.idx1-ubyte";
  bool ok1 = _read_samples( trndata, trndata1, trn_samples );

	string tstdata = mnist_folder + "/t10k-images.idx3-ubyte";
	string tstdata1 = mnist_folder + "/t10k-labels.idx1-ubyte";
  bool ok2 = _read_samples( tstdata, tstdata1, tst_samples );
  
  return ok1 && ok2;
}

Mat rotateImage(const Mat& source, double angle)
{
    Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size());
    return dst;
}

Mat1b pca( Mat1b& m ) // возвращает Mat повернутый главной осью вверх
{
  double sum_f=0;
  double sum_fx=0;
  double sum_fy=0;
  double sum_fxx=0;
  double sum_fxy=0;
  double sum_fyy=0;
  for (int y=0; y<m.cols; y++)
  {
    for (int x=0; x<m.rows; x++)
    {
      double f = m[y][x];
      sum_f += f;
      sum_fx += f*x; 
      sum_fy += f*y;
      sum_fxx += f * x * x;  
      sum_fxy += f * x * y;
      sum_fyy += f * y * y;
    }
  }
  
  double xc = sum_fx / sum_f;
  double yc = sum_fy / sum_f;
  // sum_fxx_ == sum( f * (x-xc) * (x-xc) ) == sum( f*x*x - 2*f*x*xc + f*xc*xc ) ==
  // == sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc
  double sum_fxx_ = sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc;
  double sum_fyy_ = sum_fyy - 2*sum_fy * yc  + sum_f * yc * yc;

  // fxy_ == sum( f * (x-xc) * (y-yc) ) == sum( f*x*x - f*x*yc - f*y*xc + f*xc*yc ) ==
  // == sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc
  double sum_fxy_ = sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc;

  double ds = sum_fxx_ - sum_fyy_;
  double phi_rad =  ( abs( ds ) <= 0.000001 ) ? 0. 
    : 0.5 * atan( 2*sum_fxy_ / ds );
    //: 0.5 * atan2( 2*sum_fxy_ , ds );

  double I1 = 0.5*( sum_fxx_ + sum_fyy_  + sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));
  double I2 = 0.5*( sum_fxx_ + sum_fyy_  - sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));

  double phi_degree = phi_rad * 360./ (CV_PI*2);
  Mat1b mm = rotateImage( m, phi_degree );
#if 0
  cout << phi_degree << " grad\t" << I1 << '\t'<< I2 << endl;
  imshow( "img", m );
  imshow( "img-rot", mm );
  waitKey(0);
#endif
  return mm;


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
#define  DONT_RESIZE // сильно ухудшает
#ifndef DONT_RESIZE
    if (SAMPLE_HEIGHT == 28 && SAMPLE_WIDTH == 28)
    {
      Mat1b mymat2;
      resize( mymat, mymat2, Size(16,16) ); 
      mymat = mymat2;
    }
#endif

    Mat1b mm = pca( mymat );

		samples.push_back(make_pair(label, mm));
		//samples.push_back(make_pair(label, mymat));
	}
  cout << endl << samples.size() << " samples read from " << patterns << endl;

#ifndef DONT_RESIZE
    if (SAMPLE_HEIGHT == 28 && SAMPLE_WIDTH == 28)
    {
      cout << "Samples resized to 16x16" << endl;
      SAMPLE_HEIGHT = 16;
      SAMPLE_HEIGHT = 16;
    }
#endif
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
  long long counter;

  vector< pair< int, cv::Mat > > *samples1, *samples2;
  Metr1() : samples1(0),samples2(0), counter(0) {};

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
    counter++;
    return dst;
  }
};

class Metr2 // A intersect dilated(B) + B intersect dilated(A)
{
public:
  long long counter;

  vector< pair< int, cv::Mat > > *samples1, *samples2;
  vector< pair< int, cv::Mat > > *samples1_dilated, *samples2_dilated;

  Metr2():  samples1(0), samples2(0), samples1_dilated(0), samples2_dilated(0), counter(0){}

  double computeDistance( const int& i1,  const int& i2 )  // индексы к samples[]
  {
    double dst=0;
    Mat m1 = (*samples1)[i1].second;    Mat m1ex = (*samples1_dilated)[i1].second; 
    Mat m2 = (*samples2)[i2].second;    Mat m2ex = (*samples2_dilated)[i2].second; 
    for ( int y=0; y<SAMPLE_HEIGHT; y++ )
    {
      for ( int x=0; x<SAMPLE_WIDTH; x++ )
      {
        //if (m1.at<uchar>( y, x ) == 255 && m2ex.at<uchar>( y, x ) != 255 )
        //  dst += 1;
        //if (m2.at<uchar>( y, x ) == 255 && m1ex.at<uchar>( y, x ) != 255 )
        //  dst += 1;
#if 0 // -------- good --------
        if (m1.at<uchar>( y, x ) == 255 && m2ex.at<uchar>( y, x ) != 255 )
          dst += abs( m1.at<uchar>( y, x ) - m2ex.at<uchar>( y, x ) );
        if (m2.at<uchar>( y, x ) == 255 && m1ex.at<uchar>( y, x ) != 255 )
          dst += abs( m2.at<uchar>( y, x ) - m1ex.at<uchar>( y, x ) );
#endif
#if 1
        if (m1.at<uchar>( y, x ) > m2ex.at<uchar>( y, x ) + 128)
          dst ++; ///= abs( m1.at<uchar>( y, x ) - m2ex.at<uchar>( y, x ) );
        if (m2.at<uchar>( y, x ) > m1ex.at<uchar>( y, x ) + 128)
          dst ++; ///= abs( m2.at<uchar>( y, x ) - m1ex.at<uchar>( y, x ) );
#endif
      }
    }
    counter++;
    return dst;
  }
};

void explore_cover_tree()
{
  bool test_hamming= false; //true;
  bool test_smart = true; //false;

  Metr1 ruler1;
  Metr2 ruler2;
  for (int chr =10; chr<=10; chr++)
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

    //CoverNet< int, Metr2 > cvnet2( &ruler2, SAMPLE_HEIGHT*SAMPLE_WIDTH*256, 1 ); // << 8-NN => 2.22
    CoverNet< int, Metr2 > cvnet2( &ruler2, SAMPLE_HEIGHT*SAMPLE_WIDTH*256, 8 );  // << 8-NN => 2.18, best
    //CoverNet< int, Metr2 > cvnet2( &ruler2, SAMPLE_HEIGHT*SAMPLE_WIDTH*256, 4 );  // << 8-NN => 2.24


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

      /*cout << "Test Cover Net...";
      if (cvnet2.checkCoverNet())
        cout << "ok" << endl;
      else
        cout << "failed" << endl;*/
    }

    // test recognition
    if (chr == 10)
    {
      Ticker t;
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

//#define ONLY_ONE
#ifdef ONLY_ONE
      int kNeighbours = 1; ///  ==> 2.81% error rate, ??> 159 символов в сек <?? 782 расстояния
#else
      int kNeighbours = 1; // ==> 2.81% error rate, 387 символ в сек, 782 расстояние на точку
      //int kNeighbours = 2; // ==> 2.72% error rate, 351 символ в сек, 861 расстояние на точку
      //int kNeighbours = 4; // ==> 2.34% error rate, 301 символ в сек
      //int kNeighbours = 6; // ==> 2.29% error rate, 272 символ в сек, 1041 расстояние на точку
      //int kNeighbours = 7; // ==> 2.28% error rate, 260 символ в сек, 1175 расстояние на точку
      //int kNeighbours = 8; // ==> 2.22% error rate, 250 символ в сек, 1105 расстояние на точку
      //int kNeighbours = 9; // ==> 2.24% error rate, 240 символ в сек, 1135 расстояние на точку
      //int kNeighbours = 10; // ==> 2.24% error rate, 232 символ в сек, 1162 расстояние на точку
      //int kNeighbours = 16; // ==> 2.44% error rate, 198 символ в сек, 1302 расстояние на точку
#endif

	    cout << "Test " << kNeighbours << "-NN recognition " << endl;

      ruler1.counter=0;
      ruler2.counter=0;
      for (int i_tst=0; i_tst<int(tst_samples.size()); i_tst++)
      {
        if (i_tst%500 == 0)
          cout << ".";

        double distance = SAMPLE_HEIGHT*SAMPLE_WIDTH*256; // а могли бы и отсечение указать?
        vector< pair< int, double > > nearest; 
        int i_trn = 0;
        if (0)
        {
          if (test_hamming)
            i_trn = cvnet1.findNearestPoint(i_tst, distance);
          else if (test_smart)
            i_trn = cvnet2.findNearestPoint(i_tst, distance);
        }
        else
        {
          if (test_hamming)
            nearest = cvnet1.findKNearestPoints(i_tst, kNeighbours, distance);
          else if (test_smart)
          {
            #ifdef ONLY_ONE
              int best_pt = cvnet2.findNearestPoint(i_tst, distance); 
              nearest.push_back(make_pair(best_pt, distance));
            #else
               nearest = cvnet2.findKNearestPoints(i_tst, kNeighbours, distance);
            #endif
          }
        }

		    /*imshow("test_mat", tst_samples[i_tst].second);
		    imshow("best_vertex", trn_samples[i_trn].second);
		    cerr << "Result:  tst_value = " << tst_samples[i_tst].first << " trn_value = " << trn_samples[i_trn].first << " dist = " << distance << endl;
		    cvWaitKey(0);*/

        int winner = -1;
        if (0) //kNeighbours == 1)
          winner = trn_samples[i_trn].first;
        else
        {
          vector< pair< double, int > > votes(10);
          for (int i=0;i < int(votes.size()); i++ )
          {
            votes[i].first = 0;
            votes[i].second = i;
          }

          for (int i=0; i < int(nearest.size()); i++ )
          {
            int trn_class = trn_samples[nearest[i].first].first;
            double trn_distance = nearest[i].second;
            double vote = 1. / (1. + trn_distance);
            //vote = sqrt(vote); // 2.22 => 2.25
            vote = vote*vote; // 2.22 => 2.17 !
            votes[ trn_class ].first += vote;
          }
          sort( votes.rbegin(), votes.rend() );
          winner = votes[0].second;
        }


  		  //if (tst_samples[i_tst].first == trn_samples[i_trn].first)
  		  if (winner == tst_samples[i_tst].first)
        {
          max_hit_distance = max( max_hit_distance, distance );
          hit++;
        }
        else
        {
          min_miss_distance = min( min_miss_distance, distance );
          miss++;
//#ifdef ___SHOW___
#ifdef ___SHOW___
          /*imshow("tst", tst_samples[i_tst].second);
          Mat near_mats(trn_samples[nearest[0].first].second.rows, trn_samples[nearest[0].first].second.cols * nearest.size(), trn_samples[0].second.type());
          for (int i1 = 0; i1 < nearest.size(); ++i1)
          {
            Mat Roi(near_mats, Rect(i1 * trn_samples[nearest[i1].first].second.cols, 0,trn_samples[nearest[i1].first].second.cols, trn_samples[nearest[i1].first].second.rows));
            trn_samples[nearest[i1].first].second.copyTo(Roi);
            //imshow("nya", trn_samples[nearest[i1].second].second);
          }
          imshow("nearest", near_mats);
          cvWaitKey(0);*/
#endif
        }
        

      }
      cout << "\nHits = " << hit << " \tMisses = " << miss << endl;
      cout << "Max hit dist = " << max_hit_distance << " Min miss dist = " << min_miss_distance << endl;
      cout << 10000.0 / t.dsecs() << "symbols per second" << endl;
      if (test_hamming)
        cout << ruler1.counter/10000.0 << "distance calculations per point" << endl;
      else if (test_smart)
        cout << ruler2.counter/10000.0 << "distance calculations per point" << endl;

    }
  }
#if 0
  cout << "press any key to continue" << endl;
  _getch();
#endif

}


int explore_mnist( int argc, char* argv[] )
{
#ifdef simple_tester
	test_cover_net();
#else
  string exe = argv[0];
  string mnist_folder = exe + "/../../../testdata/mnist";
	read_samples(mnist_folder);

  dilate_samples();

  explore_cover_tree();


  return 1;
///////////////////////////////////////////////////////////
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

#endif
	return 0;
}