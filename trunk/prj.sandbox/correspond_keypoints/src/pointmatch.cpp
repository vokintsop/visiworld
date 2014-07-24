#include "pointmatch.h"

#include <iostream>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

bool ReadPointMatchStorage(FileNode &root, PointMatchStorage &pms)
{
  FileNode node = root["frames"];
  if (node.empty() || !node.isSeq() || root["frameNum"].empty() ||!root["frameNum"].isInt())
  {
    cout << "Error reading pointMatch Storage" << endl;
    return false;
  }
  int nFrames = (int) root["frameNum"];
  if (pms.size() != nFrames)
    pms = PointMatchStorage(nFrames);
  cout << node.size() << endl;
  //node.
  for (FileNodeIterator iter = node.begin(); iter != node.end(); ++iter)
  {
    int frameNumber = (int) (*iter)["frameNumber"];
    if (frameNumber >= nFrames)
    {
      cout << "Error reading point Matches" << endl;
      return false;
    }
    PointMatches &pm = pms[frameNumber];
    FileNode pmNode = (*iter)["pointMatches"];
    if (pmNode.empty() || !pmNode.isSeq())
    {
      cout << "Error reading point Matches for frame " << frameNumber << endl;
      continue;
    }
    for (FileNodeIterator pmIter = pmNode.begin(); pmIter != pmNode.end(); ++pmIter)
    {
      Point lPoint, rPoint;
      try
      {
        (*pmIter)[0][0] >> lPoint.x;
        (*pmIter)[0][1] >> lPoint.y;
        (*pmIter)[1][0] >> rPoint.x;
        (*pmIter)[1][1] >> rPoint.y;
        pm.push_back(make_pair(lPoint, rPoint));
      }
      catch (Exception &e)
      {
        cout << "warining: reading point match failed: " << e.what() << endl;
      }
    }
  }
  return true;
}

bool WritePointMatchStorage(FileStorage &fs, PointMatchStorage &pms)
{
  if (!fs.isOpened())
    return false;
  fs << "frameNum" << (int) pms.size();
  fs << "frames" << "[";

  for (int iFrame = 0; iFrame < pms.size(); ++iFrame)
  {
    fs << "{";
    fs << "frameNumber" << iFrame;

    fs << "pointMatches" << "[";
    for (int i = 0; i < pms[iFrame].size(); ++i)
      fs << "[:" << pms[iFrame][i].first << pms[iFrame][i].second << "]";
    fs << "]";

    fs << "}";
  }
  fs << "]";
  return true;
}
