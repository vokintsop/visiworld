#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

void fht_vertical(cv::Mat1b &input, cv::Mat1i &outputl, cv::Mat1i &outputr);//находит матрицы fht при сдвиге влево и сдвиге вправо

void test_Hough(cv::Mat1b &input);//тестирует fht

void vertical_sum_fht_l_r(cv::Mat1i &L, cv::Mat1i &R, cv::Mat1i &result);// склеивает fhtL  и fhtR в одго изображение

int vertical_sum(cv::Mat1i &fht, int x_up, int len);// len может быть отрицательной при отрицательном сдвиге, по склеенному изображению 2 Хафов находит сумму для любой ветрикальной прымой

std::pair<cv::Point, cv::Point> find_vertical_line_from_one_pt(cv::Mat1i &fht, cv::Point a, int eps = -1);//найти наилучшую прямую через одну точку, eps != -1 значит искать в eps окрестности, в противном случае eps = fht.cols / 100;

std::pair<cv::Point, cv::Point> find_vertical_line_from_two_pt(cv::Mat1i &fht, cv::Point a, cv::Point b, int eps = -1);//найти наилучшую прямую через две точки, eps != -1 значит искать в eps окрестности, в противном случае eps = fht.cols / 100;

void test_find_vertical_line(cv::Mat1b &input);//функция для выаода наилучшей прямой через 2 точки


void count_fht(int k, cv::Mat3b input, cv::Mat1i &FHT);//строит Хафа по изображению, k - коеффициент сжатия 

std::pair<cv::Point, cv::Point> vertical_line_from_segment(int rows, cv::Point a, cv::Point b); //считает отрезок до нужной длины


//// Дальнейшее - это тестирование разного рода фильтров -- самый лучший из них будет использован в программе

void SuperFilter(cv::Mat3b &input, cv::Mat1b &res);// супер-фильтр ^_________^

void AnotherSuperFilter(cv::Mat3b &input, cv::Mat1b &res); // супер-пупер 