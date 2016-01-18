//ccdata.h -- connected components


struct CCBaseFrame /// отслеживает коробку
{
  int minx, miny, maxx, maxy; // коробка, включительно
  int height() { return maxy-miny+1; }
  int width() { return maxx-minx+1; }
  CCBaseFrame():
    minx( INT_MAX ), miny( INT_MAX ), maxx( INT_MIN ), maxy(INT_MIN )
  {}

  void add( int x, int y, double f = 1. )
  {
    minx = min( x, minx );     miny = min( y, miny ); 
    maxx = max( x, maxx );     maxy = max( y, maxy ); 
  }

};

struct CCBaseMoments1 /// отслеживает центр масс, моменты первого и второго порядка
{
  double xc, yc; // центр масс
  double sum_f;
  double sum_fx,sum_fy;
  CCBaseMoments1():    xc(0.), yc(0.), sum_f(0.), sum_fx(0.), sum_fy(0.)
  {}
  void add( int x, int y, double f = 1. )  
  {  sum_f += f;    sum_fx += f*x;     sum_fy += f*y; }
  void fix()  
  {    xc = sum_fx / sum_f;    yc = sum_fy / sum_f;  }
};

struct CCBaseMoments2 /// отслеживает центр масс, моменты первого и второго порядка
  : public CCBaseMoments1
{
  double sum_fxx, sum_fxy, sum_fyy;
  CCBaseMoments2():
    sum_fxx(0.), sum_fxy(0.), sum_fyy(0.)
  {}

  void add( int x, int y, double f = 1. )
  {
    CCBaseMoments1::add(x, y, f); 
    sum_fxx += f * x * x;    sum_fxy += f * x * y;    sum_fyy += f * y * y;
  }

  void fix()
  {
    CCBaseMoments1::fix(); ///   ==  xc = sum_fx / sum_f;    yc = sum_fy / sum_f;
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

  }
};

struct CCBase1 : public CCBaseFrame, public CCBaseMoments1 
{
  void add( int x, int y, double f = 1. )
  {
    CCBaseFrame::add(x,y,f);
    CCBaseMoments1::add(x,y,f);
  }
};

struct CCBase2 : public CCBaseFrame, public CCBaseMoments2 
{
  void add( int x, int y, double f = 1. )
  {
    CCBaseFrame::add(x,y,f);
    CCBaseMoments2::add(x,y,f);
  }
};


template< class CCBase = CCBase1 >
struct CCData_
  : public CCBase
{};

typedef CCData_<CCBase1> CCData1;
typedef CCData_<CCBase2> CCData2;



