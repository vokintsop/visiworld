#include "ocvutils/precomp.h"
#include "geomap/geomapeditor.h"


bool GeoSheet::read( const char* sheet_file_name )
{
  raster = imread( sheet_file_name ); // todo try { }
  if (raster.empty()) 
    return false;

  //imshow("GeoMap", raster);
  //waitKey(0);

  //a.xy = cv::Point(  19, 225 );  a.ns = cv::Point2d( 38.476627, 55.918096 ); // северо-западный угол трассы
  //b.xy = cv::Point( 629, 709 );  b.ns = cv::Point2d( 38.484416, 55.914618 );  // юго-восточный угол трассы
  
  return true;
}	



bool GeoMap::open( const char *sheets_list_file )
{
  string root(sheets_list_file);
  ifstream ifs( root.c_str() );
  if (!ifs.is_open())
    return false;
  int nsheets = 0;
  ifs >> nsheets;
  if (nsheets <=0 || nsheets > 1024)
    return false;
  sheets.resize(nsheets);
  for ( int i=0; i<nsheets; i++ )
  {
    GeoSheet& sh = sheets[i]; 
    string sheet_name;
    ifs >> sheet_name;
    string sheet_path = root + "/../" + sheet_name;
    if ( !sh.read( sheet_path.c_str() ))
      return false;
    ifs >> sh.a.xy.x >> sh.a.xy.y >> sh.a.en.y >> sh.a.en.x; 
    ifs >> sh.b.xy.x >> sh.b.xy.y >> sh.b.en.y >> sh.b.en.x; 
  }
  return true;
}

int GeoMap::find_best_sheet( Point2d en )
{
  double best_dd = 1.;
  int _best_sheet =-1;
  for (int i=0; i< int(sheets.size()); i++)
  {
    GeoSheet& sh = sheets[i];
    Point xy = sh.en2xy( en );
    double dx = abs( double( sh.raster.cols/2 - xy.x ) / sh.raster.cols );
    double dy = abs( double( sh.raster.rows/2 - xy.y ) / sh.raster.rows );

    if (dx > 0.5 || dy > 0.5)
      continue;
    
    double dd = max( dx, dy );
    if (dd < best_dd)
    {
      best_dd = dd;
      _best_sheet = i;
    }
  }
  return _best_sheet;
}
