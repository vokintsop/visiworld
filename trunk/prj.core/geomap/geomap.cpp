#include "ocvutils/precomp.h"
#include "geomap/geomap.h"


bool GeoMap::open(  const char* image_file  )
{
  raster = imread(image_file);
  //imshow("GeoMap", raster);
  //waitKey(0);

  //a.xy = cv::Point(  19, 225 );  a.ns = cv::Point2d( 38.476627, 55.918096 ); // ������-�������� ���� ������
  //b.xy = cv::Point( 629, 709 );  b.ns = cv::Point2d( 38.484416, 55.914618 );  // ���-��������� ���� ������
  
  return true;
}	

