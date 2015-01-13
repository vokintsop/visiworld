#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp> 

#include "cover_net2.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <time.h>

struct pt
{
  double x, y;
};
struct line
{
  double a, b, c;
  line()
  {}
  line(pt q, pt w)
  {
    a = q.y - w.y;
    b = w.x - q.x;
    c = -a * q.x - b * q.y;
  }
};

const int size_ = 100;
const int img_size = 520;

line gen_line()
{
  line l;
  l.a = l.b = l.c = 0;
  while (l.b == 0)// для упрощения считаем что они не паралллельны OY
  {
    l.a = rand() % size_ * 2 - size_ ;
    l.b = rand() % size_ * 2 - size_ ;
    l.c = rand() % size_ * 2 - size_ ;
  }
  return l;
}

pt add_rand (pt a)
{
  pt add;
  add.x = rand() % 20 - 10;
  add.y = rand() % 20 - 10;
  a.x += add.x;
  a.y += add.y;
  return a;
}
double RR(pt a)
{
  return a.x * a.x + a.y * a.y;
}

pt norm(line l)
{
  pt a;
  a.x = l.b;
  a.y = -l.a;
  if (a.x == 0)
  {
      if (a.y < 0)
      {
        a.x *= -1;
        a.y *= -1;
      }
  }
  if (a.x < 0)
  {
    a.x *= -1;
    a.y *= -1;
  }

  double r = sqrt(RR(a));
  a.x /= r;
  a.y /= r;
  return a;
}

class simpleMetr2d
{
public:
  double computeDistance( const line  l1,  line l2 ) 
  {
      pt i1 = norm(l1);
      pt i2 = norm(l2);
      return sqrt((i1.x - i2.x) * (i1.x - i2.x) + (i1.y - i2.y) * (i1.y - i2.y));
  }  
};
class angleMetr
{
public:
  double computeDistance( const line  l1, const line l2 ) 
  {
      pt i1 = norm(l1);
      pt i2 = norm(l2);
      double ck = i1.x * i2.x + i1.y * i2.y;

      //std::cerr << i1.x << " " << i2.x << " " << i1.y << " " << i2.y << " " << acos(ck) << std:: endl;
      return acos(ck);
  }  
};

line result(std::vector<pt> L) // возвращает оптимальную прямую по вектору точек
{
  angleMetr rule;
  CoverNet<line, angleMetr> cvnet(&rule, 1e5, 1);
  
  for (int i = 0; i < (int)L.size(); ++i)
  {
    for (int i1 = i + 1; i1 < (int)L.size(); ++i1)
    {
      line l(L[i], L[i1]);
      cvnet.insert(l);
    }
  }

  // теперь надо найти первое ветвление
  line res = cvnet.findFirstRamification();
  return res;
}

void draw_line (cv::Mat &img, line l, cv:: Scalar &color)
{
  pt from;
  from.x = -size_;
  from.y = (-from.x * l.a - l.c) / l.b;

  pt to;
  to.x = size_;
  to.y = (-to.x * l.a - l.c) / l.b;

  cv::line(img,cv::Point((int)(from.x + img_size), (int)(from.y + img_size)), cv::Point((int)(to.x + img_size), (int)(to.y + img_size)), color, 3);
}

void console_test()
{
  cv::Mat img(img_size * 2, img_size * 2,CV_8UC3);
  img = cv::Scalar(255, 255, 255);
  
  time_t t;
  time(&t);
  srand((unsigned int)t);
  line L = gen_line();
  draw_line(img, L, cv::Scalar(255, 0, 0));
  std::cout << "generate line: " << L.a <<" * x + " << L.b << " * y + " << L.c << std::endl;

  const int size = 15;
  std::vector<pt> points(size);
  for (int i = 0; i < size; ++i)
  {
    pt p;
    p.x = rand() % size_ * 2 - size_;
    p.y = (-p.x * L.a - L.c) / L.b;

    //std::cout << p.x << " " << p.y << std::endl;
    p = add_rand(p);
    std::cout << "points[" << i << "] = (" <<p.x + img_size << "; " << p.y + + img_size << ")" << std::endl;
   // system ("pause");
    cv:: circle(img, cv::Point((int)(p.x + img_size), (int)(p.y + img_size)), 3, cv::Scalar(0, 255, 0), 4);
  
    points[i] = p;
  }

  line ans = result(points);
  draw_line(img, ans, cv::Scalar(0, 0, 255));
  std::cout << "result line: " << ans.a <<" * x + " << ans.b << " * y + " << ans.c << std::endl;
  imshow("img", img);
  cvWaitKey(0);
}