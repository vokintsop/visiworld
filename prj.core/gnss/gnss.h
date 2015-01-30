#ifndef __GNSS_H
#define __GNSS_H
#pragma once 

#include <string>
#include <vector>


struct GNSSRecord
{
  double time; // в секундах
  // В полночь по гринвичу требуется сшивка!!!
  double nord, east;

  double yaw, pitch, roll;
  GNSSRecord():time(0),nord(0),east(0){}
  GNSSRecord( double time, double nord=0, double east=0 ):time(time),nord(nord),east(east){}

#if 0 // todo --- naming as oxts
lat:   latitude of the oxts-unit (deg)
lon:   longitude of the oxts-unit (deg)
alt:   altitude of the oxts-unit (m)
roll:  roll angle (rad),    0 = level, positive = left side up,      range: -pi   .. +pi
pitch: pitch angle (rad),   0 = level, positive = front down,        range: -pi/2 .. +pi/2
yaw:   heading (rad),       0 = east,  positive = counter clockwise, range: -pi   .. +pi
vn:    velocity towards north (m/s)
ve:    velocity towards east (m/s)
vf:    forward velocity, i.e. parallel to earth-surface (m/s)
vl:    leftward velocity, i.e. parallel to earth-surface (m/s)
vu:    upward velocity, i.e. perpendicular to earth-surface (m/s)
ax:    acceleration in x, i.e. in direction of vehicle front (m/s^2)
ay:    acceleration in y, i.e. in direction of vehicle left (m/s^2)
ay:    acceleration in z, i.e. in direction of vehicle top (m/s^2)
af:    forward acceleration (m/s^2)
al:    leftward acceleration (m/s^2)
au:    upward acceleration (m/s^2)
wx:    angular rate around x (rad/s)
wy:    angular rate around y (rad/s)
wz:    angular rate around z (rad/s)
wf:    angular rate around forward axis (rad/s)
wl:    angular rate around leftward axis (rad/s)
wu:    angular rate around upward axis (rad/s)
pos_accuracy:  velocity accuracy (north/east in m)
vel_accuracy:  velocity accuracy (north/east in m/s)
navstat:       navigation status (see navstat_to_string)
numsats:       number of satellites tracked by primary GPS receiver
posmode:       position mode of primary GPS receiver (see gps_mode_to_string)
velmode:       velocity mode of primary GPS receiver (see gps_mode_to_string)
orimode:       orientation mode of primary GPS receiver (see gps_mode_to_string)

#endif
};

std::istream & operator>>(std::istream &istr, GNSSRecord &gnss);

inline bool compareByTime( const GNSSRecord& rec, const GNSSRecord& val );

class NMEA // vector of .nmea file records (currently as it produced by akenori)
{
public:
  std::vector< GNSSRecord > records;
  NMEA(){}
  NMEA( const std::string& filename ){ load(filename); }
  bool load( const std::string& filename );
  bool loadKitti( const std::string &filename );
  void draw();
  bool getEastNord( double time, double& east, double& nord ) const;
};

#endif
