#include <opencv2/highgui/highgui.hpp>

#include "ocvgui.h"

#ifdef _WINDOWS
#include <windows.h>
#undef min
#undef max
#endif

bool setWindowText( const char* window_id, const char* window_text )
{
#ifdef _WINDOWS
#if CV_MAJOR_VERSION > 2
  //HWND hWnd = (HWND)cv::GetWindowHandle(window_id);
#else
  HWND hWnd = (HWND)cvGetWindowHandle(window_id);

  if (hWnd==NULL)
     return false;
  HWND hPar = GetParent(hWnd);
  if (hPar==NULL)
     return false;
  ::SetWindowText(hPar, window_text ); // winapi
  return true;
#endif
#endif 
  return false;
}

