// niblack.h
#include <opencv2/core/core.hpp>


int prev__niblack( cv::Mat1b& src, 
                   cv::Mat1b& res, 
                   int wing_min = 5,
                   double alpha = 0.15, 
                   bool invert = false );


int niblack( cv::Mat1b& src, 
             cv::Mat1b& res, 
             int threshold_value,
             int wing_min = 5, // 9 pels for sigma thresh, ~17 pels for final window, 
             double alpha = 0.15,
             bool invert = false );


int niblack_ex( cv::Mat1b& src, 
                cv::Mat1b& res, 
                bool invert = false );


int niblack_standart( cv::Mat1b& image, 
                      cv::Mat1b& res, 
                      int size, 
                      double k, 
                      int threshold, 
                      bool invert );