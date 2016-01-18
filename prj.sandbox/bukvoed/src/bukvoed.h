// bukvoed.h
#ifndef __BUKVOED_H
#define __BUKVOED_H

#include "bookdata.h"

class Bukvoed :
  public BookData
{
public:
  ///Bukvoed(): ruler( pages ), cvnet( &ruler, 1000000, 1 ){};
  ///int Bukvoed::addPage( const char* page_file, Rect roi=Rect() );
  int Bukvoed::browsePages();
  int Bukvoed::browseCoverNet();
};
#endif // __BUKVOED_H