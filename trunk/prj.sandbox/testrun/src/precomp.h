// precomp.h -- project default context -- starter for .cpp files
#ifndef __PRECOMP_H
#define __PRECOMP_H

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

//template< class Type, class TypeSq = Type >  // возведение в квадрат
template< class Type >  // возведение в квадрат
Type sq( const Type& a ) { return a*a; };

namespace cv {
template< class T >
bool operator < ( const cv::Point3_< T >& a, const cv::Point3_< T >& b ) 
///bool operator < ( cv::Point3_< T > a, cv::Point3_< T > b ) 
///bool operator < ( const cv::Point3d& a, const cv::Point3d& b ) 
{
  return a.x < b.x;
}
};


#endif // __PRECOMP_H
