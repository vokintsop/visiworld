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

int main( int argc, char* argv[] )
{
  cout << "markup.exe <video.avi|.mov|.mp4> [<#start_frame>]" << endl; 
  string exe  = argv[0];
  int start_frame = -1;


#if 0
  string data = "d:/testdata/glass/redman/20140630_112748_994.mp4"; ///
  start_frame = 678;
  
  //string data = "d:/testdata/glass/redman2/morn/VID_auto_20140910_174785.mp4";
  //string data = "d:/testdata/glass/redman2/morn/VID_daylight_20140910_1747846.mp4"; // ��, �������
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
  //string data = "/testdata/reika/input/reika.01/left0000.avi"; start_frame = 0; // �� ������ ����� ��������� �� ������ ���
  string data = "/testdata/reika/input/reika.01/left0001.avi";  //start_frame = 66; // �� ������, ����� �� ������ ��������
#endif

#if 0 // ������� �� ��������
  //string data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0010.avi.960.avi"; 
  string data = "/testdata/roadvideo/input/roadvideo.05/roadvideo.05.0011.avi.960.avi"; 
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
#endif

#if 1 // ������ � ������� ����� ���������� ���������
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.1920.avi"; 

  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0024.avi.960.avi"; 
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0025.avi.960.avi"; //+
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0026.avi.960.avi"; //-
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0027.avi.960.avi"; //-
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0028.avi.960.avi"; //+
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0029.avi.960.avi"; //- ... ����� ����� � �������
  ///string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0030.avi.960.avi"; //- ������������ ������� ����������� �����
  string data = "/testdata/roadvideo/input/roadvideo.01/roadvideo.01.0031.avi.960.avi"; //- �����������...

#endif


#if 0
  string data = exe + "/../../../testdata/upload/glass/20140309_124846_717.mp4";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  int start_frame = 1600; // �������� � ��������
  //int start_frame = 1750; // �������� � ��������
#endif

#if 0
  string data = exe + "/../../../testdata.upload/cities/arnhem01.mp4";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  //int start_frame = 50; // �������n��� ����� � ��������
  //int start_frame = 1600; // �������� ������������� �� �������_  
  //int start_frame = 1620; // �������� ������������� �� �������_  
  int start_frame = 1624; // �������� - ������� - �������� �� �����  
  //int start_frame = 1884; // ������� ���, �� ��������� ��� ��������� ���������
#endif

#if 0
  string data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  //int start_frame = 50; // �������n��� ����� � ��������
  //int start_frame = 1600; // �������� ������������� �� �������_  
  //int start_frame = 1620; // �������� ������������� �� �������_  
  int start_frame = 4500;///1624; // �������� - ������� - �������� �� �����  
  //int start_frame = 1884; // ������� ���, �� ��������� ��� ��������� ���������
#endif


#if 0
  string data = "/testdata/akenori/input/REC.0621/EMD00046.ts";
  // �������� ������ ��������, ���� ��� ������, ���� �������, ���� �������, ����������� ����
  //int start_frame = 50; // �������n��� ����� � ��������
  //int start_frame = 1600; // �������� ������������� �� �������_  
  //int start_frame = 1620; // �������� ������������� �� �������_  
  int start_frame = 4500;///1624; // �������� - ������� - �������� �� �����  
  //int start_frame = 1884; // ������� ���, �� ��������� ��� ��������� ���������
#endif


#if 0
  string data = exe + "/../../../testdata/upload/glass/20140309_130256_551.mp4";
  // ����������, �� ���������� �����
  int start_frame = 50;
  //int start_frame = 0;
#endif

#if 0
  string data = exe + "/../../../testdata/upload/glass/20140309_130935_445.mp4";
  // ����������, �� ���������� �����
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

