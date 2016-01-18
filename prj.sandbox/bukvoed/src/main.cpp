#include "precomp.h"
//#pragma comment(linker, "/STACK:16777216")
//#include <iostream>
//#include <conio.h>

//int run_bukvoed( int argc, char* argv[] );
#include "bukvoed.h"
#include "ocvutils/ticker.h"
int run_bukvoed( int argc, char* argv[] )
{
  Bukvoed bukvoed;

#if 0
  // set roi
  int dx =10;
  int dy = 5;
  //bukvoed.ROI = Rect( 300-dx, 263-dy, 350+2*dx, 51+2*dy );// address
  //bukvoed.ROI = Rect( 304-dx, 200-dy, 322+2*dx, 109+2*dy ); // name+address
  //bukvoed.ROI = Rect( 392-dx, 477-dy, 279+2*dx, 39+2*dy ); // dd
#endif

  Ticker t;
  //bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_zero_norm.png").c_str() ); 
  //bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_zero_bold.png").c_str() ); 
#if 0
  string filename = string(argv[0]) +"/../../../testdata/bukvoed/arial_digits_norm.png";
  Rect roi; // полная страница
  //Rect roi = Rect( 29, 15, 103, 133 ); // 00 11 -- четыре символа -- два ноля и две единицы -- ok, две сферы в ответе с 12 по 20 уровень
  // <<<>>>> Rect roi = Rect( 29, 15, 103, 641 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  //Rect roi = Rect( 29, 208, 103, 64 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  bukvoed.addPage( filename.c_str(), roi ); 
#endif

#if 0
  string filename = string(argv[0]) +"/../../../testdata/bukvoed/arial_latin_capital_norm.png";
  // 13 sizes x 26 letters == 338 -- 6,7,8,9,10,11,12,14,16,18,20,22,24
  Rect roi; // полная страница
  //Rect roi = Rect( 29, 15, 103, 133 ); // 00 11 -- четыре символа -- два ноля и две единицы -- ok, две сферы в ответе с 12 по 20 уровень
  // <<<>>>> Rect roi = Rect( 29, 15, 103, 641 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  //Rect roi = Rect( 29, 208, 103, 64 ); // <<<!!! 00 11 .. 99 -- 20 символов -- матрица 2х10 -- ??? на 19 и 20 уровнях откуда то берется 11-я сфера!!!
  bukvoed.addPage( filename.c_str(), roi ); 
#endif

#if 0 // use niblack with 3
  Rect roi = Rect( 2, 61, 238, 85 );
  string filename = string(argv[0]) +"/../../../testdata/temp2013/passport_date.png";
  bukvoed.addPage( filename.c_str(), roi ); 
#endif

#if 0
  //Rect roi = Rect(13, 79, 280, 195 );
  Rect roi = Rect(0,0, 645, 318);
  string filename = "/testdata/dibco11/hw/hw1.png";
  bukvoed.addPage( filename.c_str(), roi ); 
#endif


#if 0
  bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_digits_bold.png").c_str() ); 
  bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_digits2_norm.png").c_str() ); 
  bukvoed.addPage( (string(argv[0]) +"/../../../testdata/bukvoed/arial_digits2_bold.png").c_str() ); 
#endif

#if 1
  //bukvoed.addPage( "/books/vinokur/pages/vinokur111_salted.png" ); 
  int FIRST_PAGE=111;
  int NUM_PAGES=20;
  for (int i=FIRST_PAGE; i<FIRST_PAGE+NUM_PAGES; i++)
  {
    string filename = format( "/books/vinokur/pages/vinokur_page_%04d.png", i );
    bukvoed.addPage( filename.c_str() );
  };

  //bukvoed.addPage( "/books/vinokur/pages/vinokur111.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur112.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur113.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur114.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur115.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur116.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur117.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur118.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur119.png" ); 
  //bukvoed.addPage( "/books/vinokur/pages/vinokur120.png" ); 
#endif

  cout << "addPages ... " << t.msecs() << " milliseconds" << endl;


  bukvoed.makeIndex();
  bukvoed.browsePages();

  return 0;

}


int main( int argc, char* argv[] )
{
	int res = run_bukvoed( argc, argv );
  //std::cout << "\n\n ======= Press any key to finish... ========" << std::endl;
  //_getch();
  return res;
}