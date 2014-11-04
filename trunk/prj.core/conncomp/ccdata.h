//ccdata.h -- connected components
#include <algorithm>

class CCData
{
public:
  int flags; // пометки
#define CC_FILTERED_BY_SIZE 0x1
#define CC_FILTERED_BY_AREA 0x2  
#define CC_FILTERED_BY_RATIO 0x4  
#define CC_HIDDEN 0x8 //спрятано для показа  

#define CC_FILTERED (CC_FILTERED_BY_SIZE | CC_FILTERED_BY_AREA | CC_FILTERED_BY_RATIO)  


  int minx, miny, maxx, maxy; // коробка, включительно
  int height() { return maxy-miny+1; }
  int width() { return maxx-minx+1; }
  int area; // sum_1
  double xc, yc; // центр масс
  double sum_f;
  double sum_fx,sum_fy;
  double sum_fxx, sum_fxy, sum_fyy;
  CCData():
    flags(0), area(0),
    minx( INT_MAX ), miny( INT_MAX ), maxx( INT_MIN ), maxy(INT_MIN ),
    sum_f(0.), sum_fx(0.), sum_fy(0.), sum_fxx(0.), sum_fxy(0.), sum_fyy(0.)
  {}
  void add( int x, int y, double f = 1. )
  {
    minx = std::min( x, minx );     miny = std::min( y, miny ); 
    maxx = std::max( x, maxx );     maxy = std::max( y, maxy ); 
    area ++;
    sum_f += f;
    sum_fx += f*x; 
    sum_fy += f*y;
    sum_fxx += f * x * x;  
    sum_fxy += f * x * y;
    sum_fyy += f * y * y;
  }
  void fix()
  {
    xc = sum_fx / sum_f;
    yc = sum_fy / sum_f;
    /*
    // sum_fxx_ == sum( f * (x-xc) * (x-xc) ) == sum( f*x*x - 2*f*x*xc + f*xc*xc ) ==
    // == sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc
    double sum_fxx_ = sum_fxx - 2*sum_fx * xc  + sum_f * xc * xc;
    double sum_fyy_ = sum_fyy - 2*sum_fy * yc  + sum_f * yc * yc;

    // fxy_ == sum( f * (x-xc) * (y-yc) ) == sum( f*x*x - f*x*yc - f*y*xc + f*xc*yc ) ==
    // == sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc
    double sum_fxy_ = sum_fxy - sum_fy * xc - sum_fx * yc + sum_f * xc * yc;

    double ds = sum_fxx_ - sum_fyy_;
    double phi_rad =  ( abs( ds ) <= 0.000001 ) ? 0. 
      : 0.5 * atan( 2*sum_fxy_ / ds );
      //: 0.5 * atan2( 2*sum_fxy_ , ds );

    double I1 = 0.5*( sum_fxx_ + sum_fyy_  + sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));
    double I2 = 0.5*( sum_fxx_ + sum_fyy_  - sqrt( ds*ds + 4* sum_fxy_*sum_fxy_ ));

    double phi_degree = phi_rad * 360./ (CV_PI*2);
    */
  }

  bool filter_by_size( int min_width, int max_width,  int min_height, int max_height ) // inclusive
  {
    int w = maxx - minx + 1; 
    int h = maxy - miny + 1; 
    if (w < min_width || w > max_width || h < min_height || h > max_height)
      flags |= CC_FILTERED_BY_SIZE;
    else
      flags &= ~CC_FILTERED_BY_SIZE;
    return 0 == (flags & CC_FILTERED_BY_SIZE);
  }  
  bool filter_by_ratio( double min_w2h, double max_w2h ) // inclusive
  {
    int w = maxx - minx + 1; 
    int h = maxy - miny + 1; 
    double w2h = double(w)/(h?h:1);
    if (w2h < min_w2h || w2h > max_w2h)
      flags |= CC_FILTERED_BY_RATIO;
    else
      flags &= ~CC_FILTERED_BY_RATIO;
    return 0 == (flags & CC_FILTERED_BY_RATIO);
  } 
  bool filter_by_area( int min_area, int max_area ) // inclusive
  {
    if (area < min_area || area > max_area)
      flags |= CC_FILTERED_BY_AREA;
    else
      flags &= ~CC_FILTERED_BY_AREA;
    return 0 == (flags & CC_FILTERED_BY_AREA);
  }
};


