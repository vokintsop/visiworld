#include "ocvutils/precomp.h"
#include "markup.h"
using namespace std;


bool file_readable( const char* name )
{
  ifstream ifs( name );
  return ifs.good();
}


int markup( string& data, int start_frame )
{
  cout << "File to process " << data << endl << " Start frame " << start_frame << endl;

  if (!file_readable(data.c_str()))
  {
    cout << "no file or not readable:" << data << endl;
    return -1;
  }

  MarkupWindow markupWindow;
  markupWindow.process(data, start_frame);


  return 0;
}
////////////////////////////////////////////////////////////////
string gps_file; // gps data .gps file (produced by blackvue)

string nmea_file; // gps data .nmea file (produced by akenori)
#include "gnss/gnss.h"
NMEA NmeaFile; // возможный компаньон видео

#include "geomap/geomap.h"
GeoMap TheGeoMap("/testdata/poligon/map/pics.map"); // singleton
////////////////////////////////////////////////////////////////


void onTimer( double time )
{
  cout << time << endl;
  Point2d en(0,0);
  if (NmeaFile.getEastNord( time, en.x, en.y ))
  {
    cout << en << endl;
  }
}

int main( int argc, char* argv[] )
{
  cout << "markup.exe <video.avi|.mov|.mp4> [<#start_frame>]" << endl; 
  string exe  = argv[0];
  int start_frame = -1;


#if 0
  string data = "d:/testdata/glass/redman/20140630_112748_994.mp4"; ///
  start_frame = 678;
  
  //string data = "d:/testdata/glass/redman2/morn/VID_auto_20140910_174785.mp4";
  //string data = "d:/testdata/glass/redman2/morn/VID_daylight_20140910_1747846.mp4"; // жд, зеленый
#endif

#if 0
  //string data = "d:/testdata/glass/redman/20140630_112748_994.mp4"; ///
  //int start_frame = 678;
  string data = "d:/testdata/glass/redman/20140619_171050_033.mp4";
  int start_frame = 0;
#endif


#if 0
  string data = "d:/testdata/glass/redman/20140630_112748_994.mp4";
  int start_frame = 5200;
  //int start_frame = 0;
#endif
#if 0
  string data = exe + "/../../../testdata/upload/SGs3/VID_20140221_115030.mp4";
  int start_frame = 110;
#endif

#if 0
  string data = "/testdata/ikea/IMG_2504.MOV";
#endif

#if 0 
  //string data = "/testdata/reika/input/reika.01/left0000.avi"; start_frame = 0; // по прямой возле сбербанка за черным бмв
  string data = "/testdata/reika/input/reika.01/left0001.avi";  //start_frame = 66; // по прямой, слева по полосе обгоняют
#endif

#if 0 // следуем за лексусом
  //string data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0010.avi.960.avi"; 
  string data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0011.avi.960.avi"; 
#endif

#if 0 // следуем за лексусом
  string data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0030.avi.960.avi"; 
#endif


#if 0 
  string data = "/testdata/glass/milano/20140913_123209_598.mp4"; 
#endif

#if 1 
  string data = "/testdata/poligon/akenori/AKN00002.ts"; 
  nmea_file = "/testdata/poligon/akenori/AKN00002.nmea";
  NmeaFile.load(nmea_file);

/*
  TheGeoMap.open("/testdata/poligon/poligon1.png");
  TheGeoMap.a.xy = cv::Point(  19, 225 );  TheGeoMap.a.ns = cv::Point2d( 38.476627, 55.918096 ); // северо-западный угол трассы
  TheGeoMap.b.xy = cv::Point( 629, 709 );  TheGeoMap.b.ns = cv::Point2d( 38.484416, 55.914618 );  // юго-восточный угол трассы
*/
/*
  TheGeoMap.open("/testdata/poligon/poligon4.png");
  TheGeoMap.a.xy = cv::Point(  403, 212 );  TheGeoMap.a.ns = cv::Point2d( 38.477603, 55.918571 ); // левая штанга западного въезда в тоннель
  TheGeoMap.b.xy = cv::Point( 1130, 613 );  TheGeoMap.b.ns = cv::Point2d( 38.479553, 55.917969 );  // юго-восточный угол вагона поезда

*/

  //NmeaFile.draw();


  /*

  Mat display = TheGeoMap.raster.clone();

  Scalar col( 255,0,255, 0);
  for (int i=1; i< NmeaFile.records.size();i++)
  {
    Point2d p1( NmeaFile.records[i-1].east, NmeaFile.records[i-1].nord  );
    Point2d p2( NmeaFile.records[i].east, NmeaFile.records[i].nord  );
    Point pp1 = TheGeoMap.ns2xy( p1 );
    Point pp2 = TheGeoMap.ns2xy( p2 );
    line( display, pp1, pp2, col, 2 );
  }

  imshow("TheGeoMap trace", display );
  waitKey(0);

  */

  //return 0;
#endif

#if 0 // входим в поворот перед пешеходным переходом
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.1920.avi"; 

  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.960.avi"; //+
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0025.avi.960.avi"; //+
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0026.avi.960.avi"; //- теряет в конце огни
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0027.avi.960.avi"; //+
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0028.avi.960.avi"; //+
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0029.avi.960.avi"; //- один раз сбивается...
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0030.avi.960.avi"; //+
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0031.avi.960.avi"; //+ 

#endif


#if 0
  string data = exe + "/../../../testdata/upload/glass/20140309_124846_717.mp4";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  int start_frame = 1600; // начинаем с красного
  //int start_frame = 1750; // начинаем с зеленого
#endif

#if 0
  string data = exe + "/../../../testdata.upload/cities/arnhem01.mp4";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  //int start_frame = 50; // начинаеnтся видео с красного
  //int start_frame = 1600; // светофор переключается на зеленый_  
  //int start_frame = 1620; // светофор переключается на зеленый_  
  int start_frame = 1624; // светофор - зеленый - рассыпан на части  
  //int start_frame = 1884; // зеленый чел, но выделился как несколько компонент
#endif

#if 0
  string data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  //int start_frame = 50; // начинаеnтся видео с красного
  //int start_frame = 1600; // светофор переключается на зеленый_  
  //int start_frame = 1620; // светофор переключается на зеленый_  
  int start_frame = 4500;///1624; // светофор - зеленый - рассыпан на части  
  //int start_frame = 1884; // зеленый чел, но выделился как несколько компонент
#endif


#if 0
  string data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // подходим совсем издалека, есть вид вблизи, есть зеленый, есть круглые, комплексный тест
  //int start_frame = 50; // начинаеnтся видео с красного
  //int start_frame = 1600; // светофор переключается на зеленый_  
  //int start_frame = 1620; // светофор переключается на зеленый_  
  int start_frame = 4500;///1624; // светофор - зеленый - рассыпан на части  
  //int start_frame = 1884; // зеленый чел, но выделился как несколько компонент
#endif


#if 0
  string data = exe + "/../../../testdata/upload/glass/20140309_130256_551.mp4";
  // далековато, но контрастно виден
  int start_frame = 50;
  //int start_frame = 0;
#endif

#if 0
  string data = exe + "/../../../testdata/upload/glass/20140309_130935_445.mp4";
  // далековато, но контрастно виден
  int start_frame = 50;
  //int start_frame = 0;
#endif

#if 0
  string data = exe + "/../../../testdata/upload/iphone-munich/IMG_2394.MOV";
  int start_frame = 30;
#endif

#if 0
  string data = "/testdata/glass/pedsign001.mp4"; 
  start_frame=1001;
#endif
  if (argc > 1)
    data = exe + argv[1];
  if (argc > 2)
    start_frame = atoi(argv[2]);

  int res = markup( data, start_frame );
  return res;
}


