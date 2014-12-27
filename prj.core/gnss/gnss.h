#ifndef __GNSS_H
#define __GNSS_H

#include <string>
#include <vector>


struct GNSSRecord
{
  double time; // в секундах после первой записи
  // В полночь по гринвичу требуется сшивка!!!
  double nord, east;
  GNSSRecord():time(0),nord(0),east(0){}
  GNSSRecord( double time, double nord=0, double east=0 ):time(time),nord(nord),east(east){}
};

class NMEA // vector of .nmea file records (currently as it produced by akenori)
{
public:
  std::vector< GNSSRecord > records;
  NMEA(){}
  NMEA( const std::string& filename ){ load(filename); }
  bool load( const std::string& filename );
  void draw();
  bool getEastNord( double time, double& east, double& nord );
};


#endif
