// test cover_net on word lists
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "cover_net.h"
#include "ticker.h"

using namespace std;
using namespace cv;
vector< string > samples;

static bool read_samples( string data )
{
  ifstream input( data.c_str() );
  if (!input.is_open())
    cout << "Can't open " << data << endl;
  while (1)
  {
    string s; input >> s;
    if (s.empty())
      break;
    samples.push_back(s);
  }
  cout << samples.size() << " words read from " << data << endl;
  return true;
}

// todo:
// longest common subsequence...
// 

class Metr1Str
{
public:
  long long counter;

  vector< string > *samples1, *samples2;
  Metr1Str() : samples1(0),samples2(0), counter(0) {};

  double computeDistance( const int& i1,  const int& i2 )  // ������� � samples[]
  {
    counter++;

    double dst=0;
    string s1 = (*samples1)[i1];
    string s2 = (*samples2)[i2];
    int len1 =s1.length();
    int len2 =s2.length();
    int dlen = abs(len1-len2);
    if (len1 != len2)
      return double( dlen*20 );
    double res =0.;
    for ( int i=0; i<len1; i++ )
    {
      if (s1[i] != s2[i])
        res += 1.;
    }
    return res;
  }
};

void test_Metr1Str( int start =0, int step = 1 )
{
  Ticker t;
  Metr1Str ruler1;
  ruler1.samples1 = &samples;
  ruler1.samples2 = &samples;
  CoverNet< int, Metr1Str > cvnet1( &ruler1, 300, 1 );

  for (int i=start; i< int( samples.size() ); i+= step)
    cvnet1.insert( i );

  double sec = t.dsecs();


  cvnet1.reportStatistics( 0, 3 ); 
  cout << "Build time = " << sec << " seconds" << endl;

  //cout << "Test Cover Net...";
  //if (cvnet1.checkCoverNet())
  //  cout << "ok" << endl;
  //else
  //  cout << "failed" << endl;

}

int explore_words( int argc, char* argv[] )
{
  string exe = argv[0];
  string data = exe + "/../../../testdata/wordlists/corncob_lowercase.txt";
	if (!read_samples(data))
    return -1;

  cout << "======== Ordered list of words:" << endl;
  test_Metr1Str(0,1);


  cout << "======== Shuffled list of words:" << endl;
  std::random_shuffle(samples.begin(), samples.end());
  test_Metr1Str(0,1);
  cout << "======== Odd half list of words:" << endl;
  test_Metr1Str(1,2);
  cout << "======== Even half list of words:" << endl;
  test_Metr1Str(0,2);


  return 0;
}