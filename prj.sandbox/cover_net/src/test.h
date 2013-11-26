
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <iostream>

#include "cover_net.h"

using namespace std;

class simpleMetr
{
public:
  double computeDistance( const int& i1,  const int& i2 ) 
  {
      return abs(i1 - i2);
  }  
};

void test_cover_net ()
{ 
	 simpleMetr rule;

	 CoverNet<int, simpleMetr > cvnet(&rule, 1e5, 1);
	 vector<int> points;
	
	 cout << "BEGIN test" << endl;
	 const int NET_SIZE = 500;
	 int srand_const = cv::getTickCount();  
	// srand(-1019611072); // error 
   srand(srand_const);
   srand(-23655176);
	 for (int i = 0; i < NET_SIZE; ++i)
	 {
		 int a = rand();
		 if (i != 0)
		 {
			 double distance = 1e100;
			 int b = cvnet.findNearestPoint(a, distance);
			 int myans = 100000;
			 int anspoint = 0;
       cvnet.checkCoverNet();

			 for (int i1 = 0; i1 < points.size(); ++i1)
			 {
				 if (rule.computeDistance(points[i1], a) < myans)
				 {
					 myans = rule.computeDistance(points[i1], a);
					 anspoint = points[i1];
				 }
			 }
			 if (myans != distance)
			 {
         cerr << " Error on " << i << "step" << endl;
				 cout << "Wrong distance-- from point" << a << "  to tree" << endl;
				 cout << "returns point: " << b << "with distance: " << distance << endl;
				 cout << "correct point: " << anspoint << "with distance: " << myans << endl;

				 cout << "srand_const = " << srand_const << endl;
				 system ("pause");
			 }
		 }

		 cout << "ADD: " << a << endl;
		 cvnet.insert(a);
		 points.push_back(a);
	 }

	 cout << "END test" << endl;
}