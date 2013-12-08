#pragma comment(linker, "/STACK:16777216")
#include <iostream>
#include <conio.h>

int bukvoed( int argc, char* argv[] );


int main( int argc, char* argv[] )
{
	int res = bukvoed( argc, argv );
  //std::cout << "\n\n ======= Press any key to finish... ========" << std::endl;
  //_getch();
  return res;
}