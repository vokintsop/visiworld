// Функции генерации, общие для всех трех случаев

#include <conio.h>
#include <cassert>
#include <climits>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

double Gauss(double M, double sigma)
//генерирует нормально распределенные числа
{
	double sum = 0;
	for (int i = 0; i < 12; ++i)
	{
		sum += (rand() % 1000) * 1.0 / 1000.0;
	}

	return M + sigma * (sum - 6.0);
}

cv::Point gen_point(cv::Point c, double R)
//  генерирует числа в окружности с мат ожиданием центр и радиусом в точке
{

   cv::Point ans;
   ans.x = Gauss(c.x, R);
   ans.y = Gauss(c.y, R);

   return ans;
}


