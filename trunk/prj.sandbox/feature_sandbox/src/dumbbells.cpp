#include "dumbbells.h"
#include "init.h"

#include <ocvutils/hcoords.h> 
#include <cover_net/cover_net.h>

#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;


Point3d & Dumbbell::ComputeRotationVector()
{
  HPoint3d P1 = pivotFrame->sphere_points[matches[heads.first].second];
  HPoint3d P2 = pivotFrame->sphere_points[matches[heads.second].second];
  HPoint3d Q1 = curFrame->sphere_points[matches[heads.first].first];
  HPoint3d Q2 = curFrame->sphere_points[matches[heads.second].first];

  if (abs(norm(P2 - P1) - norm(Q2 - Q1)) >= .01)
  {
   // cout << "Dumbbell is not rigid!" << endl;
    return rotationVector = Point3d();
  }

  Point3d d1 = Q1 - P1;
  Point3d d2 = Q2 - P2;
  if (norm(d1) <= 0.001 || norm(d2) <= 0.001)
  {
    //cout << "points did not move" << endl;
    return rotationVector = Point3d();
  }
  double cosd1d2 = d1.dot(d2) / sqrt((d1.dot(d1) * d2.dot(d2)));
  Point3d n = (d1).cross(d2);
  if (cosd1d2 >= 0.99)
  {
    // cout << "dumbbell points lie on 1 meridian" << endl;
    n = P1.cross(P2).cross(Q1.cross(Q2));
  }
  /*
  double norm_n = norm(n);
  if (norm_n != 0)
  {
    n.x /= norm_n; n.y /= norm_n; n.z /= norm_n;
  }*/
  if (n.y < 0)
  {
    n.x *= -1; n.y *= -1; n.z *= -1;
  }
  else if (n.y == 0 && n.x < 0)
  {
    n.x *= -1; n.z *= -1;
  }
  else if (n.y == 0 && n.x == 0 && n.z < 0)
  {
    n.z = 1;
  }
  return rotationVector = n;
}

template<class PointType, class Metrics>
void TryClusterise(CoverNet<PointType, Metrics> &cnet)
{
  int nlevels = cnet.getCountOfLevels(); // кол-во уровней в дереве
  vector< int > true_weight;
  cnet.countTrueWeight( true_weight );
  vector< vector< pair< int, int > > > // по каждому уровню [<кол-во покрываемых точек, номер сферы>]
    cluster_candidates( nlevels ); ///, vector< pair< int, int > > ); 
  for (int i = 0; i < int(true_weight.size()); ++i) // i -- номер сферы
  {
    int level = cnet.getSphereLevel(i);
    int weight = true_weight[i];
    cluster_candidates[level].push_back( make_pair( weight, i ) );
  }
  for (int i = 0; i < cnet.getCountOfLevels(); ++i)
  {
    sort(cluster_candidates[i].rbegin(), cluster_candidates[i].rend()); // на каждом уровне сортируем по числу покрываемых точек
    
  }
  ///*
  for (int level = 1; level < cnet.getCountOfLevels(); ++level)
  {
    cout << "level " << level << ": , radius = " << cnet.getRadius(level) << endl;
    for (int i = 0; i < std::min((unsigned int) 10, cluster_candidates[level].size()); ++i)
    {
      Point3d point = *(cnet.getSphere(cluster_candidates[level].at(i).second).center);
      double tmpnorm = norm(point);
      point.x /= tmpnorm; point.y /= tmpnorm; point.z /= tmpnorm;
      cout << point << "\t norm: " << tmpnorm << ";\tweight: " << cluster_candidates[level].at(i).first << endl;
    }
    cout << endl;
  }//*/
}

void DumbbellCorrespond(Ptr<CNType> &coverNet, Ptr<SimpleFrame> &pivotFrame, Ptr<SimpleFrame> &curFrame)
{  
  if (pivotFrame == curFrame)
    return;
  //find matches
  vector<pair<unsigned int, unsigned int>> matches;
  for (unsigned int i = 0; i < curFrame->kps.size(); ++i)
  {
    double dist = 0.2;
    int iSph = coverNet->findNearestSphere(make_pair(curFrame.obj, i), dist);
    if (iSph != -1)
    {
      int ikp = coverNet->getSphere(iSph).center.second;
      matches.push_back(make_pair(i, coverNet->getSphere(iSph).center.second));
    }
  }
  
  //for all matches make a Dumbbell
  vector<Dumbbell> dumbbells;
  for (unsigned int i = 0; i < matches.size(); ++i)
    for (unsigned int j = i + 1; j < matches.size(); ++j)
    {
      Dumbbell d(matches);
      d.curFrame = curFrame;
      d.pivotFrame = pivotFrame;
      d.heads.first = i;
      d.heads.second = j;
      dumbbells.push_back(d);
    }

  cout << "\ndumbbells found: " << dumbbells.size() << endl;
  UnitSphereAnglesRuler angRuler;
  CoverNet<Point3d *, UnitSphereAnglesRuler> angCNet(&angRuler, CV_PI, CV_PI/180);
  for (unsigned int i = 0; i < dumbbells.size(); ++i)
  {
    dumbbells[i].ComputeRotationVector();
    if (dumbbells[i].rotationVector != Point3d())
      angCNet.insert(&dumbbells[i].rotationVector);
  }

  angCNet.reportStatistics();
  cout << endl;
  //поиск кластеров
  TryClusterise(angCNet);

  return;
}

void MonoCorrespondDumbbells(Ptr<FeatureDetector> featureDetector,
  Ptr<DescriptorExtractor> descriptorExtractor)
{
  string ifilename = "/testdata/roadvideo/input/roadvideo.02/roadvideo.02.0001.avi";
  VideoCapture cap(ifilename);
  int nFrames = cvRound(cap.get(CV_CAP_PROP_FRAME_COUNT));
  int w = cvRound(cap.get(CV_CAP_PROP_FRAME_WIDTH));
  int h = cvRound(cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  Mat bgr;
  Ptr<HCoords> hcoords = new HCoords(w,h);
  KeyPointDescriptorRuler ruler;
  double rootRadius = 5.0;
  double minRadius = 0.15;
  Ptr<CNType> coverNet = new CNType(&ruler, rootRadius, minRadius);
  Ptr<SimpleFrame> pivotFrame = NULL, curFrame = NULL;
  int coverNetFlush = 0;
  for (int iFrame = 0; iFrame < nFrames; ++iFrame)
  {
    char c = 0;
    if (!cap.read(bgr))
      return;
    if (!(iFrame % 4))
    {
      curFrame = new SimpleFrame(bgr, hcoords, iFrame);
      curFrame->preprocess(featureDetector, descriptorExtractor);
      if (coverNetFlush == 0)
      {
        coverNet = new CNType(&ruler, rootRadius, minRadius);
        pivotFrame = curFrame;
        InitCoverNet(coverNet, pivotFrame);
        coverNetFlush = 0;
      }
      DumbbellCorrespond(coverNet, pivotFrame, curFrame);
      curFrame->draw();
      c = cvWaitKey(0);
      ++coverNetFlush;
      coverNetFlush %= 10;
    }
    if (c == 27)
    {
      return;
    }
  }
}