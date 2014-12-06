#ifndef __GNSS_H
#define __GNSS_H

#include <string>
#include <vector>


struct GNSSRecord
{
  double time, nord, east;
  GNSSRecord():time(0),nord(0),east(0){}
};

class NMEA // vector of .nmea file records (currently as it produced by akenori)
{
public:
  std::vector< GNSSRecord > records;
  NMEA(){}
  NMEA( const std::string& filename ){open(filename);}
  bool open( const std::string& filename );
  void draw();
};


#endif
