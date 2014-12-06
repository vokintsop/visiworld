#include "gnss/gnss.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;


/*
example of .nmea record produced by akenori
---------------
$GPRMC,090450.00,A,5872.77527,N,03674.93726,E,13.517,0.07,271114,,,A*59
$GSENSOR,336,96,960
$GSENSOR,240,0,1008
$GSENSOR,320,64,912
$GSENSOR,416,-64,1008
$GSENSOR,272,48,928
$GPVTG,0.07,T,,M,13.517,N,25.033,K,A*3C
$GPGGA,090450.00,5872.77527,N,03674.93726,E,1,09,0.96,132.0,M,12.4,M,,*5C
$GPGSA,A,3,22,27,18,19,21,15,04,14,16,,,,1.56,0.96,1.23*0A
$GPGSV,4,1,13,03,29,224,,04,16,279,40,14,13,160,26,15,26,049,37*74
$GPGSV,4,2,13,16,11,224,19,18,62,082,45,19,51,291,43,21,37,104,39*79
$GPGSV,4,3,13,22,67,195,41,26,02,017,,27,64,236,37,28,01,354,23*79
$GPGSV,4,4,13,30,00,327,*4E
$GPGLL,5872.77527,N,03674.93726,E,090450.00,A,A*68
<empty line>
-----------------

5872.77527,N,03674.93726,E, --- where (58 grad 72.77527')
090450.00 --- when (utc)

http://www.radioscanner.ru/info/article166/ description


�	$GPGGA ��������� �������� GPS ������ � ��������������, ������� ����������������, �������� ������, ���������� �������������� ���������, HDOP (������ ��������� �������� �������� ���������), ���������� � ���������������� ��������� � �� �������. 
�	$GPGLL ��������� �������� GPS������� � �������������� ������, ������� � ������� ����������� ���������. 
�	$GPGSA � ���� ��������� ������������ ����� ������ GPS ��������, ��������� ���������, ������������ ��� ������� ������������� ������, ���������� ������� ���������� � ��������� $GPGGA � �������� �������� �������� ����������� ���������. 
�	$GPGSV � ��������� ����������� ���������� ������� ���������, �� ������, ����������, ������, � �������� ��������� ������/��� ��� ������� �� ���. 
�	$GPRMC ��������� RMC �������� ������ � �������, ��������������, ����� � ��������, ������������ ������������� GPS ���������. ����������� ����� ����������� ��� ����� ���������, ��������� �������� �� ������ ��������� 2 �������. ��� ���� ������ ������ ���� ������������, ���� ��� ��� ����� ������. ���������������� ���� ����� ���� ������������, ���� ������ �������� �� ������. 
�	$GPVTG ��������� VTG �������� ������� �������� ����������� ����� (COG) � �������� ������������ ����� (SOG). 
�	$GPZDA ��������� ZDA �������� ���������� � ������� �� UTC, ����������� ����, �����, ��� � ��������� ������� ����. 


*/
bool eat_comma( string& line ) // �������� ��� �� �������. ���� ������� ��� -- ������� false
{
  size_t pos = line.find_first_of(',');
  if (pos == string::npos)
    return false;
  line = line.substr( pos+1, line.length()-pos-1 );
  return true;
}

double normalize_to_decimal_grad( double x ) 
  // �� ����� �������� �����������
  // ggmm.dddd[d] ��� gg -- �������, mm.dddd ������ � ������ ������
{ // �������� � �������� � ������ ������
  double grad = int(x) / 100; // �������� ����� ����� ��������
  double minu = x - grad*100; // �������� ������ � ������� ������
  double res = grad + (minu / 60);
  return res;
} // ���� ����� ���������, ��� �� ������� �� ������ � ������ �������� ������

bool gprmc( string& line, GNSSRecord& rec )
{ //$GPRMC,090450.00,A,5872.77527,N,03674.93726,E,13.517,0.07,271114,,,A*59
  /*
  RMC � p������������ ������� GPS / ������������� ������ 
  1	2	3	4	5	6	7	8	9	10	11	12	
  $GPRMC,	Hhmmss.ss,	A,	1111.11,	A,	yyyyy.yy,	a,	x.x ,	x.x,	ddmmyy,	x.x,	A	*hh	<CR><LF> 
  1. ����� �������� �������������� UTC 
  2. ���������: � = ��������������, V = �������������� �������������� �������� 
  3,4. �������������� ������ ��������������, �����/�� 
  5,6. �������������� ������� ��������������, �����/������ (E/W) 
  7. �������� ��� ������������ (SOG) � ����� 
  8. �������� ����������� ����� � �������� 
  9. ����: dd/mm/yy 
  10. ��������� ��������� � �������� 
  11. �����/������ (E/W) 
  12. ����������� ����� ������ (�����������) 
  ������ ���������: 
  $GPRMC,113650.0,A,5548.607,N,03739.387,E,000.01,25 5.6,210403,08.7,E*69 

  */
  
  size_t pos = string::npos;
  size_t len = line.length();

  // cut command "$GPRMC,"
  string command;
  pos = line.find_first_of(',');
  if (pos != string::npos)
  {
    command = line.substr( 0, pos );
    len -= pos+1;
    line = line.substr( pos+1, len );
  } else 
    return false;

  // cut time "090450.00,"
  string utc_time;
  pos = line.find_first_of(',');
  if (pos != string::npos)
  {
    utc_time = line.substr( 0, pos );
    len -= pos+1;
    line = line.substr( pos+1, len );
  } else 
    return false;
  rec.time = atof( utc_time.c_str() );

  eat_comma(line); // skip "A,"

  string nord;
  pos = line.find_first_of(',');
  if (pos != string::npos)
  {
    nord = line.substr( 0, pos );
    len -= pos+1;
    line = line.substr( pos+1, len );
  } else 
    return false;
  rec.nord = normalize_to_decimal_grad( atof( nord.c_str() ) );

  eat_comma(line); // skip "N,"

  string east;
  pos = line.find_first_of(',');
  if (pos != string::npos)
  {
    east = line.substr( 0, pos );
    len -= pos+1;
    line = line.substr( pos+1, len );
  } else 
    return false;
  rec.east = normalize_to_decimal_grad( atof( east.c_str() ) );

  // todo .. direction etc

  return true;
}


bool NMEA::open( const std::string& filename )
{
  ifstream ifs( filename.c_str() );
  string line;
  if (!ifs.is_open())
    return false;

  while ( std::getline( ifs, line ) )
  {
    if (line.empty()) // spaces?
      continue;

    if (line.find( "$GP" ) != 0)
      continue;

    if (line.find( "$GPRMC" ) == 0) //$GPRMC,090450.00,A,5872.77527,N,03674.93726,E,13.517,0.07,271114,,,A*59
      /// �	$GPRMC ��������� RMC �������� ������ � �������, ��������������, ����� � ��������, 
      /// ������������ ������������� GPS ���������. ����������� ����� ����������� ��� ����� ���������, 
      /// ��������� �������� �� ������ ��������� 2 �������. ��� ���� ������ ������ ���� ������������, ���� ��� 
      /// ��� ����� ������. ���������������� ���� ����� ���� ������������, ���� ������ �������� �� ������. 
    {
      GNSSRecord rec;
      gprmc( line, rec );
      records.push_back(rec);
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////
// OpenCv specific part
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

void NMEA::draw()
{
  Mat3b display(300,200);
  if (records.size() < 1)
    return;

  double mi_nord = records[0].nord;
  double ma_nord = records[0].nord;
  double mi_east = records[0].east;
  double ma_east = records[0].east;
  for (int i=1; i<records.size();i++)
  {
    mi_nord = min( records[i].nord, mi_nord );
    ma_nord = max( records[i].nord, ma_nord );
    mi_east = min( records[i].east, mi_east );
    ma_east = max( records[i].east, ma_east );
  }
  
  Scalar col( 255,0,255, 0);
  double alpha =  display.cols / (ma_east-mi_east);
  double betha =  display.rows / (ma_nord-mi_nord);
  for (int i=1; i<records.size();i++)
  {
    Point2d p1( ( records[i-1].east - mi_east) * alpha, display.rows - ( records[i-1].nord - mi_nord ) * betha );
    Point2d p2( ( records[i].east   - mi_east) * alpha, display.rows - ( records[i].nord   - mi_nord ) * betha );
    line( display, p1, p2, col, 2 );
  }

  imshow("NMEA", display );
  waitKey(0);
}

