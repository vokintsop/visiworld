#include <iostream>
//#include "test.h"

int explore_mnist( int argc, char* argv[] );
int explore_words( int argc, char* argv[] );

#pragma comment(linker, "/STACK:16777216")

int main( int argc, char* argv[] )
{
	int res = explore_mnist( argc, argv );
  //testGraph();
	//int res = explore_words( argc, argv );
  system ("pause");
  return 0;
}