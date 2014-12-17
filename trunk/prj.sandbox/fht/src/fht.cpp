#include "fht.h"
using namespace std;
using namespace cv;

// shift > 0 если нужен обратный, то разворот изображения
void fht_vertical_iteration_r(Mat1b &input, Mat1i &output, int u, int d)// изначально u = 0, d = 2^deg
// output(y, x) = вес, x - координата y - shift
{
  int w = input.cols;
  if (u == d - 1)
  {
    output = Mat1i(1, w);
    for (int i = 0; i < w; ++i)
    {
      output(0, i) = input(u, i);
    }
    return;
  }
  
  Mat1i U, D;
  fht_vertical_iteration_r(input, U, u, (u + d) / 2);
  fht_vertical_iteration_r(input, D, (u + d) / 2, d);
  
  int sz = U.rows * 2;
  Mat1i result(sz, w);
  result = 0;
  
  for (int s = 0; s < sz; ++s)
  {
    for (int i = 0; i < w; ++i)
    {
      result(s, i) = U(s / 2, i);
      if (i + (s + 1) / 2 < w) 
        result(s, i) += D(s / 2, i + (s + 1) / 2);
    }
  }
  output = result;

}

void fht_vertical_iteration_l(Mat1b &input, Mat1i &output, int u, int d)// изначально u = 0, d = 2^deg
// output(y, x) = вес, x - координата y - shift
{
  int w = input.cols;
  if (u == d - 1)
  {
    output = Mat1i(1, w);
    for (int i = 0; i < w; ++i)
    {
      output(0, i) = input(u, i);
    }
    return;
  }
  
  Mat1i U, D;
  fht_vertical_iteration_l(input, U, u, (u + d) / 2);
  fht_vertical_iteration_l(input, D, (u + d) / 2, d);
  
  int sz = U.rows * 2;
  Mat1i result(sz, w);
  result = 0;
  
  for (int s = 0; s < sz; ++s)
  {
    for (int i = 0; i < w; ++i)
    {
      result(s, i) = U(s / 2, i);
      if (i - (s + 1) / 2 >= 0) 
        result(s, i) += D(s / 2, i - (s + 1) / 2);
    }
  }
  output = result;

}

void DrawLines(Mat1b &input, Mat1i &outputL, Mat1i &outputR, int trash)//рисует все линии вес которых больше чем trash
// для тестирования
{
  Mat1b draw_mat;
  input.copyTo(draw_mat);
  for (int x = 0; x < outputR.cols; ++x)
    for (int s = 0; s < outputR.rows; ++s)
	    if (outputR(s, x) > trash)
		    line(draw_mat, Point(x, 0), Point(x + s, input.rows - 1),  255, 1);
  for (int x = 0; x < outputL.cols; ++x)
    for (int s = 0; s < outputL.rows; ++s)
      if (outputL(s, x) > trash)
		    line(draw_mat, Point(x, 0), Point(x - s, input.rows - 1),  255, 1);
  imshow ("lines_more_the_tresh", draw_mat);
  //cout << draw_mat.cols << " " << draw_mat.rows << endl;
  //cvWaitKey(0);
}
void fht_vertical(Mat1b &input, Mat1i &outputl, Mat1i &outputr)
{
  
  // увеличение размера
  int st = 1;
  while (st < input.rows)
    st *= 2;
  Mat1b myInput(st, input.cols);
  myInput = 0;
  Mat roi(myInput, Rect(0,0,input.cols,input.rows));
  input.copyTo(roi);
 
  fht_vertical_iteration_l(myInput, outputl, 0, st);
  fht_vertical_iteration_r(myInput, outputr, 0, st);
  imshow("output_fht_l", outputl * 10);
  imshow("output_fht_r", outputr * 10);
  
  //imshow("output_test", out * 10);
  //cvWaitKey(0);
   DrawLines(myInput, outputl, outputr, 2000);
}