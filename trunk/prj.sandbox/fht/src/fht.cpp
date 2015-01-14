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
      result(s , i) = U(s / 2, i);
      if (/*i + (s) / 2 < w && */ i + (s + 1) / 2 < w) 
        result(s , i) += D(s / 2, i + (s + 1) / 2);
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
      if (i - (s + 1) / 2 >= 0) //&& (s + 1) / 2 < D.rows) 
        result(s, i) += D(s / 2, i - (s + 1 ) / 2);
    }
  }
  output = result;

}

void DrawLines(Mat1b &input, Mat1i &outputL, Mat1i &outputR, int trash)//рисует все линии вес которых больше чем trash
// для тестирования
{
  Mat1b draw_mat;
  input.copyTo(draw_mat);
  
  draw_mat = draw_mat * 10;
  for (int x = 0; x < outputR.cols; ++x)
    for (int s = 0; s < outputR.rows; ++s)
	    if (outputR(s, x) > trash)
		    line(draw_mat, Point(x, 0), Point(x + s, outputR.rows - 1),  255, 1);
  for (int x = 0; x < outputL.cols; ++x)
    for (int s = 0; s < outputL.rows; ++s)
      if (outputL(s, x) > trash)
		    line(draw_mat, Point(x, 0), Point(x - s, outputL.rows - 1),  255, 1);
  imshow ("lines_more_the_tresh", draw_mat);
  //cout << draw_mat.cols << " " << draw_mat.rows << endl;
  //cvWaitKey(0);
}
int test_fht_vertical_line(Mat1b& input, Point f, Point s)
{
  //cout << f.y << " " << s.y << endl;
  if (f.y == s.y - 1)
  {
    if (f.x < 0 || f.x >= input.cols)
      return 0;
    return input(f.y, f.x);
  }
  Point m;
  m.x = (f.x + s.x) / 2;
  m.y = (f.y + s.y) / 2;
  Point m1;
  m1.x = (f.x + s.x + 1) / 2;
  m1.y = (f.y + s.y) / 2;
  return test_fht_vertical_line(input, f, m1) + test_fht_vertical_line(input, m, s);
}

void test_fht_vertical_r(Mat1b &input,  Mat1i &output)
{
  int st = 1;
  while (st < input.rows)
    st *= 2;
  Mat1b myInput(st, input.cols);
  myInput = 0;
  Mat roi(myInput, Rect(0,0,input.cols,input.rows));
  input.copyTo(roi);

  output = Mat1i(st, myInput.cols);

  for (int i = 0; i < myInput.cols; ++i)
  {
    for (int i1 = 0; i1 < st; ++i1)
    {
      output(i1, i) = test_fht_vertical_line(myInput, Point(i, 0), Point(i + i1 + 1, st));
    }
  }
}

void test_fht_vertical_l(Mat1b &input,  Mat1i &output)
{
  int st = 1;
  while (st < input.rows)
    st *= 2;
  Mat1b myInput(st, input.cols);
  myInput = 0;
  Mat roi(myInput, Rect(0,0,input.cols,input.rows));
  input.copyTo(roi);

  Mat1b rev = myInput - myInput;
  Mat1i revtestl, testl;
  for (int x = 0; x < myInput.cols; ++x)
  {
    for (int y = 0; y < myInput.rows; ++y)
    {
      rev(y, x) = myInput(y, myInput.cols - x - 1); 
    }
  }
  test_fht_vertical_r(rev, revtestl);
  testl = revtestl - revtestl;
  for (int x = 0; x < myInput.cols; ++x)
  {
    for (int y = 0; y < myInput.rows; ++y)
    {
      testl(y, x) = revtestl(y, myInput.cols - x - 1); 
    }
  }

  output = testl;
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
  //imshow("output_fht_l", outputl * 10);
  //imshow("output_fht_r", outputr * 10);

  Mat1i revtestl, testl;
  
  //imshow ("testl", testl * 10);
 // cout << outputl.rows << " " << testr.rows << endl;
  //imshow("0", (testl - outputl) * 100);
  
  //imshow("output_test", out * 10);
  //cvWaitKey(0);
  // DrawLines(myInput, outputl, outputr, 4000);
   
}

void test_Haugh(Mat1b &input)
{
  Mat1i L, R;
  fht_vertical(input, L, R);
  Mat1i tstL, tstR;
  test_fht_vertical_l(input, tstL);
  test_fht_vertical_r(input, tstR);
  if (countNonZero(L != tstL) == 0 && countNonZero(R != tstR) == 0)
  {
    cout << "OK" << endl;
  }
  else
  {
    cout << "WA" << endl;
  }
}

void vertical_sum_fht_l_r(Mat1i &L, Mat1i &R, Mat1i &result)
{
  result = Mat1i(L.rows * 2, L.cols);
  result = 0;
  Mat1i roi(result, Rect(0,0,L.cols,L.rows));

  Mat1i L1 = L - L;
  for (int x = 0; x < L.cols; ++x)
  {
    for (int y = 0; y < L.rows; ++y)
    {
      L1(y, x) = L(L.rows - y - 1, x); 
    }
  }
 
  L1.copyTo(roi);
  Mat1i roi1(result, Rect(0, L.rows, L.cols, L.rows));
  R.copyTo(roi1);
 // imshow ("nn", result * 10);
 // imwrite ("C:/visiroad_3/fht.jpg", result / 10);
}

int vertical_sum(Mat1i &fht, int x_up, int len)// len может быть отрицательной при отрицательном сдвиге
{
  if (len + fht.rows / 2 >= 0 &&  len + fht.rows / 2 < fht.rows)
    return fht(len + fht.rows / 2, x_up);
  return -1;
}

pair<Point, Point> vertical_line_from_segment(int rows, Point a, Point b)
{
  if (a.y == b.y)
  {
    return make_pair(Point(-1, -1), Point(-1, -1));
  }
  if (a.y > b.y)
    swap(a, b);
  Point p;
  Point up, down;
  Point delta = a - b;
  double k = 1.0 * b.y / (b.y - a.y);
  
  delta.x *= k;
  delta.y *= k;
  up.y = b.y + delta.y;
  up.x = b.x + delta.x;
  

  delta = b - a;
  k = 1.0 * (rows - a.y) / delta.y;
  delta.x *= k;
  delta.y *= k;
  down.y = a.y + delta.y;
  down.x = a.x + delta.x;

  return make_pair(up, down);
}

int count_weight(Mat1i &fht, Point a, Point b)
{
  if (a.y == b.y) // точно не вертикальная
    return -1;
  
  pair<Point, Point> p = vertical_line_from_segment(fht.rows / 2, a, b);
  int s = p.second.x - p.first.x;
  return vertical_sum(fht, p.first.x, s);
}
pair<Point, Point> find_vertical_line_from_one_pt(Mat1i &fht, Point a, int eps)
{
  if (eps == -1)
  {
    eps = fht.cols / 100;
  }
  int W = -1;
  pair<Point, Point> res = make_pair(a, a);

  for (int i = a.x - eps; i <= a.x + eps; ++i)
  {
    for (int i1 = -fht.rows; i1 <= fht.rows; ++i1)
    {
      int w = count_weight(fht, Point(i, a.y), Point(i + i1, fht.rows));
      //cout << w << endl;
      if (w > W)
      {
        W = w;
        res = vertical_line_from_segment(fht.rows / 2, Point(i, a.y), Point(i + i1, fht.rows));
      }
    }
  }
  return res;
}
pair<Point, Point> find_vertical_line_from_two_pt(Mat1i &fht, Point a, Point b, int eps)
{
  if (eps == -1)
  {
    eps = fht.cols / 10;
  }
  int W = -1;
  pair<Point, Point> res = make_pair(a, b);

  for (int i = a.x - eps; i <= a.x + eps; ++i)
  {
    for (int i1 = b.x - eps; i1 <= b.x + eps; ++i1)
    {
      int w = count_weight(fht, Point(i, a.y), Point(i1, b.y));
      if (w > W)
      {
        W = w;
        res = vertical_line_from_segment(fht.rows / 2, Point(i, a.y), Point(i1, b.y));
      }
    }
  }
  return res;
}


Point fromTestP1;
Point fromTestP2;
Mat1i fht;
Mat1b inputTest;
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "find line in pt(" << x << ", " << y << ")" << endl;
        fromTestP1.x = x;
        fromTestP1.y = y;
        pair<Point, Point>  p = find_vertical_line_from_one_pt(fht, fromTestP1);
        line(inputTest, p.first, p.second, 255);
        imshow("line", inputTest);
        cvWaitKey(100);
    }
    if  ( event == EVENT_RBUTTONDOWN )
    {
        cout << "find line in pt(" << fromTestP1.x <<  ","  << fromTestP1.y << ") pt(" << x << ", " << y << ")" << endl;
        fromTestP2.x = x;
        fromTestP2.y = y;

        pair<Point, Point>  p = find_vertical_line_from_two_pt(fht,  fromTestP1, fromTestP2);
        line(inputTest, p.first, p.second, 255, 5);
        imshow("line", inputTest);
        cvWaitKey(100);
    }

}    

void test_find_vertical_line(Mat1b &input)
{
  inputTest = input;
  imshow("line", inputTest);
  Mat1i L, R;
  fht_vertical(input, L, R);
  //DrawLines(input, L, R, 5000);
  Mat1i haugh;
  vertical_sum_fht_l_r(L, R, haugh);
  //imshow ("L", L * 10);
  //imshow ("R", R * 10);
  imshow("m_input", input * 10);
  fht = haugh;
  setMouseCallback("m_input", CallBackFunc, NULL);
  //cvWaitKey(100);
}