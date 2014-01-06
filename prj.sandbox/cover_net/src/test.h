
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <vector>
#include <iostream>
#include <queue>
#include <cmath>

#include "cover_net2.h"

using namespace std;

struct pt
{
  double x, y;
};

class simpleMetr
{
public:
  double computeDistance( const int& i1,  const int& i2 ) 
  {
      return abs(i1 - i2);
  }  
};

class simpleMetr2d
{
public:
  double computeDistance( const pt  i1,  pt i2 ) 
  {
      return sqrt((i1.x - i2.x) * (i1.x - i2.x) + (i1.y - i2.y) * (i1.y - i2.y));
  }  
};


/*void test_cover_net ()
{ 
	 simpleMetr rule;

	 CoverNet<int, simpleMetr > cvnet(&rule, 1e5, 1);
	 vector<int> points;

   int k = 3; // количество ближайших, которое ищем
	
	 cout << "BEGIN test" << endl;
	 const int NET_SIZE = 1000;
	 long long srand_const = cv::getTickCount();  
	// srand(-1019611072); // error 
   srand((unsigned int)srand_const);
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
*/
void checkGraph(CoverNet<pt, simpleMetr2d > &cvnet)
{
  vector<vector<int> > M(cvnet.spheres.size(), vector<int>(cvnet.spheres.size(), 0)); // матрица смежности графа
  for (int i = 0; i < (int)cvnet.spheres.size(); ++i)
  {
    for (int i1 = 0; i1 < (int)cvnet.spheres.size(); ++i1)
    {
      if (cvnet.computeDistance(i, cvnet.spheres[i1].center) != 0 && cvnet.spheres[i].level == cvnet.spheres[i1].level && cvnet.computeDistance(i, cvnet.spheres[i1].center) <= cvnet.getRadius(cvnet.spheres[i].level) * 2.0)
        M[i][i1] = 1;
      //cout << M[i][i1];
    }
    //cout << endl;
  }
  for (int i = 0; i < (int)cvnet.graphes.size(); ++i)
  {
    for (int i1 = 0; i1 < (int)cvnet.graphes[i].vertex.size(); ++i1)
    {
      for (int v = cvnet.graphes[i].vertex[i1]; v != -1; v = cvnet.graphes[i].elems[v].next)
      {
        --M[i1][cvnet.graphes[i].elems[v].vert_num];
        //cout << "^_^" << endl;
      }
    }
  }
  for (int i = 0; i < (int)cvnet.spheres.size(); ++i)
  {
    for (int i1 = 0; i1 < (int)cvnet.spheres.size(); ++i1)
    {
      if (M[i][i1] != 0)
      {
        cout << "Error at edge " << i << " " << i1 << endl; //либо есть ребро, где его быть не должно, лиюо, наоборот, не хватает ребра
        if (M[i][i1] < 0)
          cout << "+" << endl;// думает, что есть несуществующие ребра
        else
          cout << "-" << endl;// надо чтобы оно было, а его нету
      }
    }
  }
}

void testGraph()
{  
	 simpleMetr2d rule;

	 CoverNet<pt, simpleMetr2d > cvnet(&rule, 1e5, 1);
	 vector<pt> points;

   cout << "BEGIN test" << endl;
	 const int NET_SIZE = 100;

   for (int i = 0; i < NET_SIZE; ++i)
	 {
		  pt a;
      a.x = rand() % 1000;
      a.y = rand() % 1000;
      cvnet.insert(a);
   }
   
   cout << "end building graph" << endl;
   checkGraph(cvnet);
}