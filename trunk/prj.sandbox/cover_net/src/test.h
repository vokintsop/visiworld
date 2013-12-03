
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <iostream>
#include <queue>

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

   int k = 3; // количество ближайших, которое ищем
	
	 cout << "BEGIN test" << endl;
	 const int NET_SIZE = 1000;
	 long long srand_const = cv::getTickCount();  
	// srand(-1019611072); // error 
   srand(srand_const);
   srand(118227059);
   set<int> S;

	 for (int i = 0; i < NET_SIZE; ++i)
	 {
		 int a = rand();
     if (S.find(a)!= S.end())
       continue;
     S.insert(a);
		 if (i != 0)
		 {
			 double distance = 1e100;
			 vector<pair<int, double> > b = cvnet.findKNearestPoints(a, k, distance);
			 int anspoint = 0;
       cvnet.checkCoverNet();

       priority_queue<pair<double, int> > Q; // совсем ленивая проверка

			 for (int i1 = 0; i1 < int( points.size() ); ++i1)
			 {
         Q.push(make_pair(rule.computeDistance(points[i1], a) * (-1), points[i1]));
			 }

       vector<pair<int, double> > myans;
       while (true)
       {
         if (myans.size() == k || Q.empty())
           break;
         pair<double, int> pt = Q.top();
         Q.pop();
         myans.push_back(make_pair(pt.second, pt.first * (-1)));
         //cout << pt.second << endl;
       }

       bool check = true;
       if (b.size() != myans.size())
         check = false;
       else
       {
         for (int i = 0; i < int( b.size() );++i)
         {
           if (myans[i].second != b[i].second)
             check = false;
         }
       }

			 if (!check)
			 {
         cout << " Error on " << i << "step" << endl;
  
         cout << "count points :" << endl;
         for (int i1 = 0; i1 < int( b.size() ); ++i1)
         {
           cout << "(" << b[i1].first << ", " << b[i1].second << ") "; 
         }
         cout << endl;

         cout << "correct points :" << endl;
         for (int i1 = 0; i1 < int( myans.size() ); ++i1)
         {
           cout << "(" << myans[i1].first << ", " << myans[i1].second << ") "; 
         }
         cout << endl;
				 cout << "srand_const = " << srand_const << endl;
				 system ("pause");
			 }
		 }

		// cout << "ADD: " << a << endl;
		 cvnet.insert(a);
		 points.push_back(a);
	 }

	 cout << "END test" << endl;
}
