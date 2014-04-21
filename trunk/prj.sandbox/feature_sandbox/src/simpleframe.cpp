#include "simpleframe.h"
#include <string>
#include <map>
using namespace cv; 
using namespace std;


void SimpleFrame::preprocess(Ptr<FeatureDetector> detector, Ptr<DescriptorExtractor> extractor)
{
  detector->detect(src, kps);
  extractor->compute(src, kps, descriptors);
  project();
}