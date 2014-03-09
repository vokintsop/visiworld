// synt test DEgenerator
#include "precomp.h"

void  testrun_points2lines_2d( const string& input_folder,  const string& output_folder ); // фолдеры могут совпадать
void  testrun_points2points_2d( const string& input_folder,  const string& output_folder );

int main( int argc, char* argv[] )
{
  string exe = argv[0];
  string testdata = exe + "/../../../testdata/";
  //testrun_points2points_2d( testdata + "points2points_2d/clu%03d" ,   testdata + "points2points_2d/clu%03d" );
  testrun_points2lines_2d( testdata + "points2lines_2d/line%03d" ,   testdata + "points2lines_2d/line%03d" );
 // testrun_lines2points_2d( exe +  "/../../../testdata/lines2points_2d/" );

 
	return 0;
}