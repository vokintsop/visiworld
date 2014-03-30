// Функции генерации, общие для всех трех случаев

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

inline double Gauss_(double M, double sigma)
//генерирует нормально распределенные числа
{
	double sum = 0;
	for (int i = 0; i < 12; ++i)
	{
		sum += (rand() % 1000) * 1.0 / 1000.0;
	}

	return M + sigma * (sum - 6.0);
}

inline cv::Point gen_point(cv::Point c, double R)
//  генерирует числа в окружности с мат ожиданием центр и радиусом в точке
{
   cv::Point ans;
   ans.x = (int)Gauss_(c.x, R);
   ans.y = (int)Gauss_(c.y, R);

   return ans;
}

inline cv::Point3d gen_point_on_sphere(cv::Point3d c, double R)
// генирует точку на единичной  сфере, отклоненную от данной точки с на сфере, в гауссовом распределении
{
  cv::Point3d z = Point3d(0, 0, 1);
  double u = Gauss_(0, R);
  cv::Point3d xz = Point3d(sin(u), 0, cos(u));
  cv::Point3d add;
  double alpha = rand();
  add.x = xz.x * cos(alpha) + xz.y * sin(alpha);
  add.y = -xz.x * sin(alpha) + xz.y * cos(alpha);
  add.z = xz.z;
  Point3d res = c + add - z;
 // cout << c.x << " " << c.y << " " << c.z << endl;
 // cout << res.x << " " << res.y << " " << res.z << endl;
 // cout << endl;
  return res;
}