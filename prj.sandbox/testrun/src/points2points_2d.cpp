// 2d points clusters DEgenerator
#include "precomp.h"

void  testrun_points2points_2d( const string& input_template,  const string& output_template ) // фолдеры могут совпадать
{
  int num_tests=8;
  for (int test=1; test<num_tests; test++)
  {
    string input_name = format( input_template.c_str(), test ) + ".png";
    Mat1b img = imread( input_name, IMREAD_GRAYSCALE );
    imshow(input_name, img);
    waitKey(0);
  }
}
