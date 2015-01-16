#include "ocvutils/precomp.h"

#include "geomap/geomapeditor.h"
#include "markup/markupeditor.h"
#include "soundui/soundui.h"
using namespace std;

// the single app
Ptr< GeoMapEditor >  pGeoMapEditor; // singleton
Ptr< MarkupEditor >  pMarkupEditor;

bool iskitti = false;  // ��������������� � true ���� �������� �� ���������� �������� kitti

/////////////////////////////////////////////////////////////// ���� ��� ��������

string gps_file; // gps data .gps file (produced by blackvue)

string nmea_file; // gps data .nmea file (produced by akenori)
#include "gnss/gnss.h"
NMEA theNmeaFile; // ��������� ��������� �����
////////////////////////////////////////////////////////////// ���� ��� ��������

int markup( string& data, int start_frame ) 
// ����, ��� ���������� �����/�������� ��������
// � ����� start_frame, -1 �������� "����������� ���"
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
  if (en != en_prev)
  {
    pGeoMapEditor->update_location(en);
  }

}

#define IGNORE_COMMAD_LINE true
//#define IGNORE_COMMAD_LINE false   //// ���������� ��������� -- ����� ����������� �� ���� ��� ���������� ///

bool setup( int argc, char* argv[], string& data, int& start_frame )
{
  if (argc > 1 && ! IGNORE_COMMAD_LINE ) // ������� ����� ���������
  {
    string exe  = argv[0];
    string argv1 = argv[1];
    bool filepass_is_absolute = ( argv1.length() >=2 && (argv1[0] == '/' || argv1[1] == ':') ); // ��������
    if (filepass_is_absolute)
      data = argv[1];
    else
      data = exe + argv[1];

    if (argc > 2)
      start_frame = atoi(argv[2]); 

    return true;
  }

  ///// ������ ��� ���������� /////

#if 0
  data = "d:/testdata/glass/redman/20140630_112748_994.mp4"; ///
  start_frame = 678;
  
  //data = "d:/testdata/glass/redman2/morn/VID_auto_20140910_174785.mp4";
  //data = "d:/testdata/glass/redman2/morn/VID_daylight_20140910_1747846.mp4"; // ��, �������
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
  //data = "/testdata/reika/input/reika.01/left0000.avi"; start_frame = 0; // �� ������ ����� ��������� �� ������ ���
  data = "/testdata/reika/input/reika.01/left0001.avi";  //start_frame = 66; // �� ������, ����� �� ������ ��������
#endif

#if 0 // ������� �� ��������
  //data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0010.avi.960.avi"; 
  data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0011.avi.960.avi"; 
#endif

#if 0 // ������� �� ��������
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

#if 0 // ������ � ������� ����� ���������� ���������
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.1920.avi"; 

  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0025.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0026.avi.960.avi"; //- ������ � ����� ����
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0027.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0028.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0029.avi.960.avi"; //- ���� ��� ���������...
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0030.avi.960.avi"; //+
  ///data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0031.avi.960.avi"; //+ 

#endif


#if 0
  data = exe + "/../../../testdata/upload/glass/20140309_124846_717.mp4";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  int start_frame = 1600; // �������� � ��������
  //int start_frame = 1750; // �������� � ��������
#endif

#if 0
  data = exe + "/../../../testdata.upload/cities/arnhem01.mp4";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  //int start_frame = 50; // �������n��� ����� � ��������
  //int start_frame = 1600; // �������� ������������� �� �������_  
  //int start_frame = 1620; // �������� ������������� �� �������_  
  int start_frame = 1624; // �������� - ������� - �������� �� �����  
  //int start_frame = 1884; // ������� ���, �� ��������� ��� ��������� ���������
#endif

#if 0
  data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  //int start_frame = 50; // �������n��� ����� � ��������
  //int start_frame = 1600; // �������� ������������� �� �������_  
  //int start_frame = 1620; // �������� ������������� �� �������_  
  int start_frame = 4500;///1624; // �������� - ������� - �������� �� �����  
  //int start_frame = 1884; // ������� ���, �� ��������� ��� ��������� ���������
#endif


#if 0
  data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  //int start_frame = 50; // �������n��� ����� � ��������
  //int start_frame = 1600; // �������� ������������� �� �������_  
  //int start_frame = 1620; // �������� ������������� �� �������_  
  int start_frame = 4500;///1624; // �������� - ������� - �������� �� �����  
  //int start_frame = 1884; // ������� ���, �� ��������� ��� ��������� ���������
#endif


#if 0
  data = exe + "/../../../testdata/upload/glass/20140309_130256_551.mp4";
  // ����������, �� ���������� �����
  int start_frame = 50;
  //int start_frame = 0;
#endif

#if 0
  data = exe + "/../../../testdata/upload/glass/20140309_130935_445.mp4";
  // ����������, �� ���������� �����
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

  if (iskitti)
  {
    pGeoMapEditor = new GeoMapEditor("/testdata/kitti/map"); 
  }
  else
  {
    pGeoMapEditor = new GeoMapEditor("/testdata/poligon/map"); 
  }
  
  pMarkupEditor = new MarkupEditor(iskitti);

  int res = markup( data, start_frame );

  dbgPressAnyKey();
  return res;
}


