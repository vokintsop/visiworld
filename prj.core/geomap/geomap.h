#ifndef __GEOMAP_H
#define __GEOMAP_H

struct Reper
{
  cv::Point     xy;
  cv::Point2d   ns;
};

struct GeoSheet
{
  cv::Mat3b raster;
  Reper a,b;

  cv::Point ns2xy( cv::Point2d ns )
  {
    return Point( 
      0.5 + a.xy.x + (ns.x-a.ns.x) * (b.xy.x-a.xy.x)/(b.ns.x-a.ns.x), 
      0.5 + a.xy.y + (ns.y-a.ns.y) * (b.xy.y-a.xy.y)/(b.ns.y-a.ns.y));
  }

  GeoSheet() {}
  GeoSheet( const char* sheet_file_name ) {  read( sheet_file_name ); }
  bool read(  const char* sheet_file_name );

};

class GeoMap
{
public:
  std::vector< GeoSheet > sheets;

public:
  GeoMap(){}
  GeoMap( const char* sheets_list_file ) { open( sheets_list_file ); }
  bool open(  const char* sheets_list_file  );	

  void draw( cv::Point2d en );
};

#endif