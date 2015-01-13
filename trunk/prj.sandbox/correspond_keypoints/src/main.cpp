#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <iostream>

#include "simpleframe.h"
#include "correspond.h"
#include "pointmatch.h"
#include <cover_net/cover_net.h>
#include <map>



using namespace cv;
using namespace std;

string featureType;
map<string, int> featureDetectorInts;
map<string, double> featureDetectorDoubles;
map<string, bool> featureDetectorBools;
cv::Ptr<FeatureDetector> featureDetector;
cv::Ptr<DescriptorExtractor> descriptorExtractor;


///*
void FillStandartFeatureDetectorParameters()  
{
  featureType = "SURF";

  featureDetectorInts["extended"] = 0;
  featureDetectorInts["upright"] = 1; //no rotation
  //featureDetectorInts["nOctaves"] = 16; //default 4
  //featureDetectorInts["nOctaveLayers"] = 8; //default 2

  featureDetectorDoubles["hessianThreshold"] = 2000;
}//*/
/*
void FillStandartFeatureDetectorParameters()  
{
  featureType = "SIFT";
  featureDetectorInts["nFeatures"] = 400;
  featureDetectorDoubles["contrastThreshold"] = 0.04; //default 0.04
  featureDetectorDoubles["edgeThreshold"] = 10.0; // default 10.0
  featureDetectorDoubles["sigma"] = 1.6; // default 1.6
}
//*/

/*
void FillStandartFeatureDetectorParameters()
{
  featureType = "BRISK";
  featureDetectorInts["thresh"] = 30; //default 30
  featureDetectorInts["octaves"] = 30; //default 30
  featureDetectorBools["nonmaxSuppression"] = true; //default true
}*/

void InitFeatureDetector()
{
  FillStandartFeatureDetectorParameters();
  initModule_nonfree();
  featureDetector = FeatureDetector::create(featureType);
  for (map<string, int>::const_iterator iter = featureDetectorInts.begin();
    iter != featureDetectorInts.end();
    ++iter)
    featureDetector->setInt(iter->first, iter->second);
  for (map<string, double>::const_iterator iter = featureDetectorDoubles.begin();
    iter != featureDetectorDoubles.end();
    ++iter)
    featureDetector->setDouble(iter->first, iter->second);
  for (map<string, bool>::const_iterator iter = featureDetectorBools.begin();
    iter != featureDetectorBools.end();
    ++iter)
    featureDetector->setBool(iter->first, iter->second);
}

void InitDescriptorExtractor()
{
  //descriptorExtractor = DescriptorExtractor::create("SURF");//featureType);
  descriptorExtractor = DescriptorExtractor::create(featureType);
}

void Init()
{
  InitFeatureDetector();
  InitDescriptorExtractor();
}



int main( int argc, char** argv )
{
  Init();
    
  string filenameFormat = "/testdata/reika/input/reika.02/%s%04d.avi";
  int iEpisode = 0;
  
  VideoCapture lcap(format(filenameFormat.c_str(), "left", iEpisode)), rcap(format(filenameFormat.c_str(), "right", iEpisode));
  FileStorage fs(format(filenameFormat.c_str(), "", iEpisode) + ".xml", FileStorage::WRITE);
  int nFrames = cvRound(lcap.get(CV_CAP_PROP_FRAME_COUNT));
 
  Mat lbgr;
  Mat rbgr;
  KeyPointDescriptorRuler ruler;
  Ptr<SimpleFrame> lFrame = NULL, rFrame = NULL;
  PointMatchStorage pms;

  for (int iFrame = 0; iFrame < nFrames; ++iFrame)
  {
    char c = 0;
    if (!lcap.read(lbgr) || !rcap.read(rbgr))
      return 0;
    lFrame = new SimpleFrame(lbgr, iFrame);
    rFrame = new SimpleFrame(rbgr, iFrame);
    lFrame->preprocess(featureDetector, descriptorExtractor);
    rFrame->preprocess(featureDetector, descriptorExtractor);
    //keypoints detected
    PointMatches pm;
    CorrespondStereo(&ruler, lFrame, rFrame, pm);
    pms.push_back(pm);
    //return 0;
    c = cv::waitKey(1);
    if (c == 27)
    {
      WritePointMatchStorage(fs, pms);
      //fs.release();
      return 0;
    }
    c = 0;
  }
  WritePointMatchStorage(fs, pms);
  fs.release();
  cout << "Ended Succesfully!!" << endl;
  return 0;
}

