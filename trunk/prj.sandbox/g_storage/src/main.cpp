#include <iostream>

//#include "geomap.h"
//#include "kdtree.h"

#include "gstorage.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main( int argc, char* argv[] )
{
  GStorage gst;
  gst.load("/testdata/kitti/map");  

  GStorageIndex gsti;
  gsti.attach(&gst);

//   GMObject gmo(ENPoint2d(8.4237, 49.011));
// 
//   std::vector<int> indices1;
//   std::vector<double> dists1;
//   gsti.radiusSearch(gmo, 1, 0.0002, indices1, dists1);
// 
//   GMObject* p_gmoF1 = gst[indices1[0]];
// 
//   std::vector<int> indices2;
//   std::vector<double> dists2;
//   gsti.nearestKSearch(gmo, 2, indices2, dists2);
// 
//   GMObject* p_gmoF2_1 = gst[indices2[0]];
//   GMObject* p_gmoF2_2 = gst[indices2[1]];

  return 0;
}