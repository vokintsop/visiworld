#include "ocvutils/precomp.h"

#include "geomap/geomapeditor.h"
#include "markup/markupeditor.h"
#include "maptorealworld.h"
#include "soundui/soundui.h"
using namespace std;

// the single app
Ptr< GeoMapEditor >  pGeoMapEditor; // singleton
Ptr< MarkupEditor >  pMarkupEditor;
Ptr< Camera2DPoseEstimator> pCamPoseEst;

bool iskitti = false;  // устанавливается в true если работаем со структурой датасета kitti

/////////////////////////////////////////////////////////////// дело для разборок

string gps_file; // gps data .gps file (produced by blackvue)

string nmea_file; // gps data .nmea file (produced by akenori)
#include "gnss/gnss.h"
NMEA theNmeaFile; // возможный компаньон видео
////////////////////////////////////////////////////////////// дело для разборок

int markup( string& data, int start_frame ) 
// итак, нам заказывают показ/просмотр разметки
// с кадра start_frame, -1 означает "разберитесь мол"
{
  cout << "File to process " << data << endl << " Start frame " << start_frame << endl;

  /*
  if (!file_readable(data.c_str()))
  {
    cout << "no file or not readable:" << data << endl;
    return -1;
  }
  */


  pMarkupEditor->process(data, start_frame);

  return 0;
}


//GeoMapEditor theGeoMapEditor("/testdata/kitti/map"); // singleton

////////////////////////////////////////////////////////////////


void onTimer( double time )
{
  cout << time << endl;
  Point2d en(0,0);
static Point2d en_prev(0,0);
  if (theNmeaFile.getEastNord( time, en.x, en.y ))
  {
    cout << en << endl;
  }
  CameraOnMap cam = pCamPoseEst->GetPoseAtTime(time);
  if (en != en_prev)
  {
    pGeoMapEditor->update_location(en, cam.direction);
  }
  
  Mat img = pMarkupEditor->GetBaseImageCopy();
  if (img.empty())
    return;
  vector<Point2d> enPoints;
  pGeoMapEditor->exportObjPoints(enPoints);

  //CameraOnMap cam = pCamPoseEst->GetPoseAtTime(time);
  drawMapPointsOnImage(enPoints, cam, img);
  imshow("sticks_demonstration", img);
}

#define IGNORE_COMMAD_LINE true
//#define IGNORE_COMMAD_LINE false   //// нормальное состояние -- смело исправляйте на него при надобности ///

bool setup( int argc, char* argv[], string& data, int& start_frame )
{
  if (argc > 1 && ! IGNORE_COMMAD_LINE ) // указаны явные параметры
  {
    string exe  = argv[0];
    string argv1 = argv[1];
    bool filepass_is_absolute = ( argv1.length() >=2 && (argv1[0] == '/' || argv1[1] == ':') ); // ванильно
    if (filepass_is_absolute)
      data = argv[1];
    else
      data = exe + argv[1];

    if (argc > 2)
      start_frame = atoi(argv[2]); 

    return true;
  }

  ///// запуск без параметров /////

#if 0
  data = "d:/testdata/glass/redman/20140630_112748_994.mp4"; ///
  start_frame = 678;
  
  //data = "d:/testdata/glass/redman2/morn/VID_auto_20140910_174785.mp4";
  //data = "d:/testdata/glass/redman2/morn/VID_daylight_20140910_1747846.mp4"; // жд, зеленый
#endif

#if 0
  //data = "d:/testdata/glass/redman/20140630_112748_994.mp4"; ///
  //int start_frame = 678;
  data = "d:/testdata/glass/redman/20140619_171050_033.mp4";
  int start_frame = 0;
#endif


#if 0
  data = "d:/testdata/glass/redman/20140630_112748_994.mp4";
  int start_frame = 5200;
  //int start_frame = 0;
#endif
#if 0
  data = exe + "/../../../testdata/upload/SGs3/VID_20140221_115030.mp4";
  int start_frame = 110;
#endif

#if 0
  data = "/testdata/ikea/IMG_2504.MOV";
#endif

#if 0 
  //data = "/testdata/reika/input/reika.01/left0000.avi"; start_frame = 0; // по прямой возле сбербанка за черным бмв
  data = "/testdata/reika/input/reika.01/left0001.avi";  //start_frame = 66; // по прямой, слева по полосе обгоняют
#endif

#if 0 // следуем за лексусом
  //data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0010.avi.960.avi"; 
  data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0011.avi.960.avi"; 
#endif

#if 0 // следуем за лексусом
  data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0030.avi.960.avi"; 
#endif


#if 0 
  data = "/testdata/glass/milano/20140913_123209_598.mp4"; 
#endif

#if 0
  data = "/testdata/poligon/input/bvu.01/20141127_121836_N.mp4"; 
  nmea_file = "/testdata/poligon/input/bvu.01/20141127_121836_N.gps";
  theNmeaFile.load(nmea_file);
#endif

#if 1
  //data = "/testdata/kitti/2011_09_26/2011_09_26_drive_0001";
  //data = "/testdata/kitti/2011_09_26/2011_09_26_drive_0002";
  data = "/testdata/kitti/2011_09_26/2011_09_26_drive_0005";
  //data = "/testdata/kitti/2011_09_26/2011_09_26_drive_0048";
  iskitti = true;
  theNmeaFile.loadKitti(data);
#endif

#if 0 
  data = "/testdata/poligon/akenori/AKN00002.ts"; 
  nmea_file = "/testdata/poligon/akenori/AKN00002.nmea";
  theNmeaFile.load(nmea_file);
#endif

#if 0 // входим в поворот перед пешеходным переходом
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.1920.avi"; 

  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0025.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0026.avi.960.avi"; //- теряет в конце огни
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0027.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0028.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0029.avi.960.avi"; //- один раз сбивается...
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0030.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0031.avi.960.avi"; //+ 

#endif


#if 0
  data = exe + "/../../../testdata/upload/glass/20140309_124846_717.mp4";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  int start_frame = 1600; // начинаем с красного
  //int start_frame = 1750; // начинаем с зеленого
#endif

#if 0
  data = exe + "/../../../testdata.upload/cities/arnhem01.mp4";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  //int start_frame = 50; // начинаеnтся видео с красного
  //int start_frame = 1600; // светофор переключается на зеленый_  
  //int start_frame = 1620; // светофор переключается на зеленый_  
  int start_frame = 1624; // светофор - зеленый - рассыпан на части  
  //int start_frame = 1884; // зеленый чел, но выделился как несколько компонент
#endif

#if 0
  data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  //int start_frame = 50; // начинаеnтся видео с красного
  //int start_frame = 1600; // светофор переключается на зеленый_  
  //int start_frame = 1620; // светофор переключается на зеленый_  
  int start_frame = 4500;///1624; // светофор - зеленый - рассыпан на части  
  //int start_frame = 1884; // зеленый чел, но выделился как несколько компонент
#endif


#if 0
  data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  //int start_frame = 50; // начинаеnтся видео с красного
  //int start_frame = 1600; // светофор переключается на зеленый_  
  //int start_frame = 1620; // светофор переключается на зеленый_  
  int start_frame = 4500;///1624; // светофор - зеленый - рассыпан на части  
  //int start_frame = 1884; // зеленый чел, но выделился как несколько компонент
#endif


#if 0
  data = exe + "/../../../testdata/upload/glass/20140309_130256_551.mp4";
  // далековато, но контрастно виден
  int start_frame = 50;
  //int start_frame = 0;
#endif

#if 0
  data = exe + "/../../../testdata/upload/glass/20140309_130935_445.mp4";
  // далековато, но контрастно виден
  int start_frame = 50;
  //int start_frame = 0;
#endif

#if 0
  data = exe + "/../../../testdata/upload/iphone-munich/IMG_2394.MOV";
  int start_frame = 30;
#endif

#if 0
  data = "/testdata/glass/pedsign001.mp4"; 
  start_frame=1001;
#endif
  return true;
}


int main( int argc, char* argv[] )
{  
  cout << "markup.exe <video.avi|.mov|.mp4> [<#start_frame>]" << endl; 
  int start_frame = -1;
  string data;

  if (!setup( argc, argv, data, start_frame ))
    return -1;
  
  Mat intrinsics;
  if (iskitti)
  {
    pGeoMapEditor = new GeoMapEditor("/testdata/kitti/map"); 
    intrinsics = (Mat_<double>(3, 3) << 984.2439, 0.000000, 690.0000,
                                    0.000000, 980.8141, 233.1966,
                                    0.000000, 0.000000, 1.000000);
  }
  else
  {
    pGeoMapEditor = new GeoMapEditor("/testdata/poligon/map"); 
    intrinsics = (Mat_<double>(3, 3) << 997.89280, 0.00000, 1013.89921,
	                                  0.00000, 918.43316, 594.69025,
		                                0.00000, 0.00000, 1.00000 );
  }
  
  pMarkupEditor = new MarkupEditor(iskitti);
  pCamPoseEst = new Camera2DPoseEstimator(theNmeaFile, intrinsics);

  int res = markup( data, start_frame );

  //dbgPressAnyKey();
  return res;
}


