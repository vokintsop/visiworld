#include "pose.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "ocvutils/precomp.h"

using namespace cv;

bool CameraPose::FillExtrinsics()
{
  //double delta = 0;
  //double sign = 1;
  //double angle = 0.5 * M_PI  - asin(direction[0]);
  //Vec3d rot_vec(0, delta + sign * angle, 0);
  double yaw = -acos(direction[0]) + 0.5 * M_PI; //������������ yaw
  //yaw ������������� �� ������� ������� �� ����������� �� �����.
  //��� �������� � ������� �������� ����� ����������� ������ ������� ������� � 
  // �� ��� �
  // ��� ��� ��� Y ������������ ����� :0.5 * M_PI - yaw
  // ������ ������� ��� ��� Y ������������ ����.
  Vec3d rot_vec(0, -0.5 * M_PI + yaw, 0);
  Rodrigues(rot_vec, R);
  return true;
}

Matx34d CameraPose::GetProjectionMatrix()
{
  //return (I|0) * (R|t)
  Matx34d initMat;
  hconcat(intrinsics, cv::Mat::zeros(3, 1, Mat(intrinsics).type()), initMat);
  Matx34d tmp;
  Matx44d transMat;
  hconcat(R, t, tmp);
  Matx14d botLine(0, 0, 0, 1);
  vconcat(tmp, botLine, transMat);

  return initMat * transMat;
}