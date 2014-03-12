// synt test generator
#include "precomp.h"
#include "gen.h"
void  testgen_points2points_2d( string res_folder );
void  testgen_points2lines_2d( string res_folder );
void  testgen_lines2points_2d( string res_folder );
void  testgen_points3d_line_vconst( string res_folder );


void testGauss()
{
  const int xMax = 1000;
  const int yMax = 1000;
  const int sigma = 20; // радиус кластера, сигма

  Mat1b img(yMax, xMax);

  img = 0;

  int _sigma = 50;
  vector<int> X(xMax, 0);
  for(int i = 0; i < 50000; ++i)
  {
     int x = Gauss_(xMax / 2, _sigma);
     if (x >= 0 && x < xMax)
     {
      X[x] += 1;
      int y = 500-X[x] < 0 ? 0 : 500-X[x];
      img(y, x) = 255;
     }
  }
  line(img, Point( xMax / 2 +_sigma, 0 ), Point( xMax / 2 +_sigma, yMax ), Scalar(128) );
  line(img, Point( xMax / 2 -_sigma, 0 ), Point( xMax / 2 -_sigma, yMax ), Scalar(128) );
  imshow("img", img);
  cvWaitKey(0);
}

int main( int argc, char* argv[] )
{
  //testGauss();
  string exe = argv[0];
  testgen_points2points_2d( exe +  "/../../../testdata/points2points_2d/" );
  testgen_points2lines_2d( exe +  "/../../../testdata/points2lines_2d/" );
  testgen_lines2points_2d( exe +  "/../../../testdata/lines2points_2d/" );
  testgen_points3d_line_vconst( exe +  "/../../../testdata/points3d_line_vconst/" );
 
	return 0;
}