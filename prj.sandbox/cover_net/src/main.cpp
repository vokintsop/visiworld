#include <iostream>
#include <conio.h>

int explore_mnist( int argc, char* argv[] );
int explore_words( int argc, char* argv[] );


int main( int argc, char* argv[] )
{
	int res = explore_mnist( argc, argv );
	//int res = explore_words( argc, argv );
  std::cout << "\n\n ======= Press any key to finish... ========" << std::endl;
  _getch();
  return res;
}