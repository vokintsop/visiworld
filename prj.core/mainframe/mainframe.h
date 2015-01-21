#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "geomap/geomapeditor.h"
#include "markup/markupeditor.h"
#include "coords/maptorealworld.h"
//... как вариант .. //#include <map>

class MarkupMainFrame // центральный синглетон разметчика
{
public:
  Ptr< GeoMapEditor >  pGeoMapEditor; // разметчик карты (вид сверху)
  Ptr< MarkupEditor >  pMarkupEditor; // разметчик кадра (вид с камеры... какой? сбоку)
  Ptr< Camera2DPoseEstimator> pCamPoseEst; // ув€зывает карту с ...ЁЁЁ... главной камерой системы

  int waitKey( int delay );

};

#endif // __MAINFRAME_H