// niblack.h

int niblack( 
  cv::Mat1b& src, 
  cv::Mat1b& res, 
  int wing_min = 5, // 9 pels for sigma thresh, ~17 pels for final window, 
  double alpha = 0.2,
  bool invert=false );
