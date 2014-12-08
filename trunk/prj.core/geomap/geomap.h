#ifndef __GEOMAP_H
#define __GEOMAP_H

struct Reper
{
  cv::Point     xy;
  cv::Point2d   ns;
};

class GeoMap
{
public:
  cv::Mat3b raster;
  Reper a,b;

  cv::Point GeoMap::ns2xy( cv::Point2d ns )
  {
    return Point( 
      0.5 + a.xy.x + (ns.x-a.ns.x) * (b.xy.x-a.xy.x)/(b.ns.x-a.ns.x), 
      0.5 + a.xy.y + (ns.y-a.ns.y) * (b.xy.y-a.xy.y)/(b.ns.y-a.ns.y));
  }

public:
  GeoMap(){}
  GeoMap( const char* image_file ) { open( image_file ); }
  bool open(  const char* image_file  );	
};

#endif