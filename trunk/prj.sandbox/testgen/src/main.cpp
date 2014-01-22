// synt test generator
#include "precomp.h"
const int xMax = 1000;
const int yMax = 1000;
const int sigma = 20; // радиус кластера, сигма

void  testgen_points2lines_2d( string res_folder );

int main( int argc, char* argv[] )
{
  string exe = argv[0];
  //testgen_points2points_2d( exe +  "/../../../testdata/testgen/points2points_2d/" );
  testgen_points2lines_2d( exe +  "/../../../testdata/testgen/points2lines_2d/" );
  //testgen_lines2points_2d( exe +  "/../../../testdata/testgen/lines2points_2d/" );

 
	return 0;
}