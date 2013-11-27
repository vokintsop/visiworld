// test cover_net on word lists
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "cover_net.h"

using namespace std;
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

int words( int argc, char* argv[] )
{
  string exe = argv[0];
  string data = exe + "/../../../testdata/wordlists/corncob_lowercase.txt";
	if (!read_samples(data))
    return -1;
  return 0;
}