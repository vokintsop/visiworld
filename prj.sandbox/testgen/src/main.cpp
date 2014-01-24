// synt test generator
#include "precomp.h"
#include "gen.h"
const int xMax = 1000;
const int yMax = 1000;
const int sigma = 20; // радиус кластера, сигма

void  testgen_points2points_2d( string res_folder );
void  testgen_points2lines_2d( string res_folder );


void testGauss()
{
  Mat1b img(yMax, xMax);
  img = 0;

  vector<int> X(xMax, 0);
  for(int i = 0; i < 5000; ++i)
  {
     int x = Gauss_(xMax / 2, sigma);
     if (x >= 0 && x < xMax)
     {
      X[x] += 1;
      img(X[x], x) = 255;
     }
  }
  imshow("img", img);
  cvWaitKey(0);
}

int main( int argc, char* argv[] )
{
  testGauss();
  //string exe = argv[0];
  //testgen_points2points_2d( exe +  "/../../../testdata/testgen/points2points_2d/" );
  //testgen_points2lines_2d( exe +  "/../../../testdata/testgen/points2lines_2d/" );
  //testgen_lines2points_2d( exe +  "/../../../testdata/testgen/lines2points_2d/" );

 
	return 0;
}