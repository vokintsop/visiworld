#include "simpleframe.h"

#include <string>
#include <map>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
using namespace cv; 
using namespace std;


void SimpleFrame::preprocess(Ptr<FeatureDetector> detector, Ptr<DescriptorExtractor> extractor)
{
  int kpsRoiYSkew = 200;
  Rect kpsRoi = Rect(Point(0,kpsRoiYSkew), Size(src.cols, src.rows - kpsRoiYSkew));
  ///*
  Mat mask = Mat::zeros(src.size(), CV_8U);
  mask(kpsRoi).setTo(255);
  detector->detect(src, kps, mask);
  extractor->compute(src, kps, descriptors);//*/
  
  /*
  detector->detect(src(kpsRoi), kps);
  for(int i = 0; i < kps.size(); ++i)
    kps[i].pt.y += kpsRoiYSkew;
  extractor->compute(src, kps, descriptors);//*/

  Mat meanDescr = Mat::zeros(descriptors.row(0).size(), descriptors.type());
  cout << "descriptors depth: " << descriptors.depth()  << ", descriptors size (rows, cols) = " << descriptors.rows << ", " << descriptors.cols << endl;
  for (int i = 0; i < descriptors.rows; ++i)
  {
    meanDescr += descriptors.row(i) / static_cast<double> (descriptors.rows);
  }
  cout << "mean descriptors norm = " << norm(meanDescr) << endl;
  for (int i = 0; i < descriptors.rows; ++i)
  {
    descriptors.row(i) -= meanDescr;
    sigmaDescr += pow(norm(descriptors.row(i)), 2.0) / static_cast<double> (descriptors.rows);
    
  }
  sigmaDescr = sqrt(sigmaDescr);
  cout << "sigma = " << sigmaDescr << endl;
}




void SimpleFrame::draw(string window_name)
{
  if (window_name.empty())
    window_name = "detected_points";
  Mat draw_mat = src.clone();
  for (unsigned int i = 0; i < kps.size(); ++i)
  {
    circle(draw_mat, kps[i].pt, 2, cv::Scalar(255,0,0), 2);
    //circle(draw_mat, kps[i].pt, kps[i].size, cv::Scalar(255,0,0));
  }

  Mat draw_mat_resized;
  resize(draw_mat, draw_mat_resized, cv::Size(), 0.5, 0.5);
  imshow(window_name, draw_mat_resized);
}