// precomp.h -- project default context -- starter for .cpp files


#include <conio.h>
#include <cassert>
#include <climits>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

#include "ocvutils/ocvutils.h"

inline void dbgPressAnyKey()
{
#ifdef _DEBUG
  printf("\n\nPress any key...");
  _getch();
#endif
}

inline bool __false( std::string message = "" )
{
  cout << message;
  dbgPressAnyKey();
  return false;
}

inline bool __true( std::string message = "" )
{
  cout << message;
  return true;
}

inline bool file_readable( const char* name )
{
  ifstream ifs( name );
  return ifs.good();
}


