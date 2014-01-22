// ������� ���������, ����� ��� ���� ���� �������

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

double Gauss_(double M, double sigma)
//���������� ��������� �������������� �����
{
	double sum = 0;
	for (int i = 0; i < 12; ++i)
	{
		sum += (rand() % 1000) * 1.0 / 1000.0;
	}

	return M + sigma * (sum - 6.0);
}

cv::Point gen_point(cv::Point c, double R)
//  ���������� ����� � ���������� � ��� ��������� ����� � �������� � �����
{

   cv::Point ans;
   ans.x = Gauss_(c.x, R);
   ans.y = Gauss_(c.y, R);

   return ans;
}