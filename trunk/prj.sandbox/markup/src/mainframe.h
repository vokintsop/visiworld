#ifndef __MAINFRAME_H
#define __MAINFRAME_H

#include "geomap/geomapeditor.h"
#include "markup/markupeditor.h"
#include "maptorealworld.h"

class MarkupMainFrame
{
public:
  Ptr< GeoMapEditor >  pGeoMapEditor; // singleton
  Ptr< MarkupEditor >  pMarkupEditor;
  Ptr< Camera2DPoseEstimator> pCamPoseEst;
};

#endif // __MAINFRAME_H