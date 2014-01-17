#include <iostream>
#include "simpletest.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int explore_mnist( int argc, char* argv[] );
int explore_words( int argc, char* argv[] );

#pragma comment(linker, "/STACK:16777216")

int main( int argc, char* argv[] )
{
  explore_mnist(argc, argv);
  return 0;
}