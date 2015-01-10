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
#include <opencv2/imgproc/imgproc.hpp> 

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

inline bool __false( std::string message = "__false():" )
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


inline string name_and_extension( string filename )
{
  // Remove directory if present.
  // Do this before extension removal incase directory has a period character.
  const size_t last_slash_idx = filename.find_last_of("\\/");
  if (std::string::npos != last_slash_idx)
  {
    filename.erase(0, last_slash_idx + 1);
  }
  return filename;
}

///////////// windows specific (todo -- make for linux)
#ifdef _WINDOWS
#include <windows.h>
#undef min
#undef max
#endif // _WINDOWS

inline void set_window_text( const char* title, const char* text )
{
#ifdef _WINDOWS
  HWND hWnd = (HWND)cvGetWindowHandle(title);
	HWND hPar = GetParent(hWnd);
  SetWindowText(hPar, text );
#endif 
}

inline bool ensure_folder( string folder )
{
#ifdef _WINDOWS
  if (CreateDirectory(folder.c_str(), NULL) ||
    ERROR_ALREADY_EXISTS == GetLastError())
    return true;
#endif
  cout << "Can't create directory " << folder << endl;
  return false;
}

