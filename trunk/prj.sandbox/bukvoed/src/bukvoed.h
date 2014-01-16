// bukvoed.h
#ifndef __BUKVOED_H
#define __BUKVOED_H

#include "pagedata.h"
#include "cover_net.h"



struct CoverPoint
{
  int iPage; // ����� ��������, ������ � pages[]
  int iCC;  // ����� ���������� ���������, ������ � pages[iPage].cc[]
  CoverPoint( int iPage, int iCC ): iPage( iPage ), iCC( iCC ) {}
  bool operator == ( const CoverPoint & other ) const { return iPage == other.iPage && iCC == other.iCC; }
};


inline bool more_128( int x, int y, Mat1b& m ) // white
{
  if ( x < m.cols && x>=0 && y < m.rows && y >= 0 )
    return m[y][x] > 128;
  return true; // ������� ������� ��� �����
  //return x < m.cols && x>=0 && y < m.rows && y >= 0 && m[y][x] > 128;
}

////////////////////////////////////////////////////////////////////////////////////
class Metr�� // A intersect dilated(B) + B intersect dilated(A)
{
public:
  long long counter; // 
  vector< PageData >& pages;
  Metr��( vector< PageData >& pages ): pages(pages), counter(0){}

  double computeDistance( const CoverPoint& cp1,  const CoverPoint& cp2 )  // ������� � ��[]
  {
    PageData& pd1 = pages[cp1.iPage];    
    PageData& pd2 = pages[cp2.iPage];
    CCData& ccd1 = pd1.cc[cp1.iCC];            
    CCData& ccd2 = pd2.cc[cp2.iCC];

    double dst=0;
    int dx = cvRound( ccd2.xc - ccd1.xc );
    int dy = cvRound( ccd2.yc - ccd1.yc );
    for ( int y=ccd1.miny; y<ccd1.maxy; y++ )
    {
      for ( int x=ccd1.minx; x<ccd1.maxx; x++ )
      {
        if (pd1.labels[y][x] == cp1.iCC) // ������
        {
          int qqq = pd1.src[y][x];
          //?assert( pd1.src[y][x] < 128 );  // ����� -- ������ -- ������ -- 00.
          if (more_128( x+dx, y+dy, pd2.src_dilated ))
            dst++;
        }
      }
    }

    for ( int y=ccd2.miny; y<ccd2.maxy; y++ )
    {
      for ( int x=ccd2.minx; x<ccd2.maxx; x++ )
      {
        if (pd2.labels[y][x] == cp2.iCC) // ������
        {
          //?assert( pd2.src[y][x] < 128 );
          if (more_128( x-dx, y-dy, pd1.src_dilated ))
            dst++;
        }
      }
    }

    counter++;
    return dst;
  }
};


class Bukvoed
{
  vector< PageData > pages;
  Metr�� ruler;
  CoverNet< CoverPoint, Metr�� > cvnet;
public:
  Bukvoed(): ruler( pages ), cvnet( &ruler, 1000000, 1 ){};
  int Bukvoed::addPage( const char* page_file );
  int Bukvoed::browse();
  void Bukvoed::makeIndex();
};
#endif // __BUKVOED_H