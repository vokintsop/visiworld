#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "geomap/geomapeditor.h"
#include "markup/markupeditor.h"
#include "coords/maptorealworld.h"
//... ��� ������� .. //#include <map>

class MarkupMainFrame // ����������� ��������� ����������
{
public:
  Ptr< GeoMapEditor >  pGeoMapEditor; // ��������� ����� (��� ������)
  Ptr< MarkupEditor >  pMarkupEditor; // ��������� ����� (��� � ������... �����? �����)
  Ptr< Camera2DPoseEstimator> pCamPoseEst; // ��������� ����� � ...���... ������� ������� �������

  int waitKey( int delay );

};

#endif // __MAINFRAME_H