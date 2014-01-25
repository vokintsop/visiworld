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

using namespace std;
using namespace cv;


//template< class Type, class TypeSq = Type >  // возведение в квадрат
template< class Type >  // возведение в квадрат
Type sq( const Type& a ) { return a*a; };


#endif // __PRECOMP_H
