#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "geomap/geomapeditor.h"
#include "markup/markupeditor.h"
#include "coords/maptorealworld.h"
//... ��� ������� .. //#include <map>

class MarkupMainFrame // ����������� ��������� ����������
{
public:
  cv::Ptr< GeoMapEditor >  pGeoMapEditor; // ��������� ����� (��� ������)
  cv::Ptr< MarkupEditor >  pMarkupEditor; // ��������� ����� (��� � ������... �����? �����)
  cv::Ptr< Camera2DPoseEstimator> pCamPoseEst; // ��������� ����� � ...���... ������� ������� �������

  int waitKey( int delay );

};

#endif // __MAINFRAME_H