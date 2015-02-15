#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

void fht_vertical(cv::Mat1b &input, cv::Mat1i &outputl, cv::Mat1i &outputr);//������� ������� fht ��� ������ ����� � ������ ������

void test_Hough(cv::Mat1b &input);//��������� fht

void vertical_sum_fht_l_r(cv::Mat1i &L, cv::Mat1i &R, cv::Mat1i &result);// ��������� fhtL  � fhtR � ���� �����������

int vertical_sum(cv::Mat1i &fht, int x_up, int len);// len ����� ���� ������������� ��� ������������� ������, �� ���������� ����������� 2 ����� ������� ����� ��� ����� ������������ ������

std::pair<cv::Point, cv::Point> find_vertical_line_from_one_pt(cv::Mat1i &fht, cv::Point a, int eps = -1);//����� ��������� ������ ����� ���� �����, eps != -1 ������ ������ � eps �����������, � ��������� ������ eps = fht.cols / 100;

std::pair<cv::Point, cv::Point> find_vertical_line_from_two_pt(cv::Mat1i &fht, cv::Point a, cv::Point b, int eps = -1);//����� ��������� ������ ����� ��� �����, eps != -1 ������ ������ � eps �����������, � ��������� ������ eps = fht.cols / 100;

void test_find_vertical_line(cv::Mat1b &input);//������� ��� ������ ��������� ������ ����� 2 �����


void count_fht(int k, cv::Mat3b input, cv::Mat1i &FHT);//������ ���� �� �����������, k - ����������� ������ 

std::pair<cv::Point, cv::Point> vertical_line_from_segment(int rows, cv::Point a, cv::Point b); //������� ������� �� ������ �����


//// ���������� - ��� ������������ ������� ���� �������� -- ����� ������ �� ��� ����� ����������� � ���������

void SuperFilter(cv::Mat3b &input, cv::Mat1b &res);// �����-������ ^_________^

void AnotherSuperFilter(cv::Mat3b &input, cv::Mat1b &res); // �����-����� 