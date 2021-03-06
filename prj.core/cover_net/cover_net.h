// cover_net.h
//#ifdef false

#ifndef __COVER_NET_H
#define __COVER_NET_H

#include <vector>
#include <algorithm>
#include <cassert>
#include <queue>
#include <iomanip>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <queue>
#include <cstdio>

#define COVER_NET_VERBOSE
//#define COVER_NET_VERBOSE_DETAILED

template< class PointType >
struct PointsCluster // �� ���� CoverSphere ������ ���������� �� PointsCluster, �������� ����������� ��������
{
  PointType center;
  double radius;
  double weight;
  PointsCluster():radius(0),weight(0){};
  PointsCluster(  PointType center, double radius, double weight ):
    center(center), radius(radius),weight(weight)
    {};
};

template < class PointType>
struct CoverSphere
{
  int parent;    // �������� // fat -- �� ����������� ��� �������� ��������
  int prev_brother;   // ������ ����������� �����; 0 -- ���
  int last_kid;  // ��������� �� ���������� ���������� �������; 0 -- ���

  //... ?
  double distance_to_parent; // ���������� �� ������������ �������  // fat?
  //std::vector< int > ancles; // ������� ������� ����, ������� ��������� ������ // fat fat slow
  int ancle; // ������ ������ ������ ������ --> ancles[ancle](����� � ������ ���� � ������� ����������� ����������?)
  //.....

  PointType center;
  int points;   // ���������� �����, ��������������� � ������ �������� // fat
  int level;    // ������� ������� � ������ // fat

  CoverSphere( 
    int level,
    const PointType& center,
    int parent, 
    double distance_to_parent
    ):
    parent(parent),
    center(center), 
    level(level),
    prev_brother(0),
    last_kid(0),
    ancle(-1),
    points(1),
    distance_to_parent(distance_to_parent)
  {
  } 

  void print(int level)
  {
    std::string indent;
    for (int i=0; i<level; i++)
      indent.push_back('\t');

///   need to define '<<' operator on PointType /// std::cout << indent << center;
    std::cout  << " parent=" << parent << " last_kid=" << last_kid << " prev_brother=" << prev_brother;

    std::cout << std::endl;
  }
};

struct CoverRecord // ����������� ���������� ����������
{
  double minDistance;
  int sphereIndex;
  int sphereLevel;
};

template < class PointType, class Metrics >
class CoverNet
{
  std::vector< CoverSphere< PointType > > spheres;
  std::vector<std::pair< int, int > > ancles; // <ancle sphere, next> ������ ������ ������� ����, ������� ��������� ������ 

  Metrics* ruler;
  double rootRadius;  // ��� �������� ���� ���������� ����������� ��������� ���������� ����� �������� ���������� �������
                      // ���� ����������� �����, ��������� �� ���� �� ���������� ������ rootRadius ������ �������������� �����
  double minRadius;   // ���� ���������� �� ������ ����� ������ minRadius ��������� ����� �� �����������
  double maxRadius;   // ���� ���������� �� ������ ����� ������ maxRadius ����� ������������

  int iLastSphere;  // ����� �����, � ������� ���� ��������� ��� ������������ �����
  int iLastSphereLevel; // 0 -- ���
  //double lastComputedDistance; // �������� ���������� ������������ ����������

  ////int iRootSphere;  // ������ ���������, _������������_ ������ "���������������� ���������" 
  ////int iRootLevel;   // ������� ����� "���������������� ���������"
  //// ������� ���������������� ��������� ������� � ����������� �������� ������ ������� ������� ���������,
  //// ��� ���������� ����� ���������� ���������� ���������� ������� 

  int attemptsToInsert; // ������� ����� ������� �������
  int rejectedInserts; // ������� ����� ������� ������� ������� ������� �����

  double squeezeRatio; // ����������� ���������� ��������� �����������

  std::vector< double > levelsRadii;
public:
  int getCountOfLevels() { return int( levelsRadii.size() ); }
  int getSphereLevel( int i_sphere ) { return spheres[i_sphere].level; }
  double getRadius( int level ) { return levelsRadii[level]; }
  double getMinRadius( int level ) { return minRadius; }
  //std::vector< double > levelsMinRadii;


  CoverNet( Metrics* ruler, double rootRadius, double minRadius, 
    double squeezeRatio = 0.5, // ����������� ���������� ��������� �����������
    double maxRadius = -1 )
    : ruler(ruler), rootRadius(rootRadius), minRadius(minRadius),
      iLastSphere(-1), iLastSphereLevel(-1)
  {
    if (maxRadius == -1)
     maxRadius = rootRadius; 

    double rad = rootRadius;
    for (int i=0; i<256; i++)
    {
      levelsRadii.push_back( rad );
      if (rad < minRadius)
        break;
      rad *= squeezeRatio;
    }

    attemptsToInsert=0; // ������� ����� ������� �������
    rejectedInserts=0; // ������� ����� ������� ������� ������� ������� �����
  };

  int insert( 
    // ��������� �����, ���������� ������������� ����� ����� ������� ������, � ������� ����������� ��� �������; 
    // ���������� 0 ���� ����� ����� �� ��������� ������ ����� ��������� (������� ����)
    const PointType& pt // ����������� ����� 
    );

  void countTrueWeight(std::vector<int> &result); // ��������� ��� ������ ����� ������� ����� ��� ��������� �� ����� ����
 
private: int countTrueWeight_(int fromNum, double fromRad, int num);// �������� ��� ���������� ����, fromNum - ����� �����, ��� ������� ��������� ���, num - ����� ������������ �����

private:
  enum { SPHERE_CREATED, POINT_ATTACHED };
  void  notifyParents( const PointType& pt, int iSphere, int iSphereLevel, int eventToNote  );

//#define DONT_FORCE_DIRECT_SUCCESSORS
  int makeSphere( // ������� �����, �� ������ ����������� � ���������� �� �����
    int iSphereLevel, // ������� ����������� �����
    const PointType& pt, // ����� �����
    int parent, // ������������ ����� (�������������, ��� ����� ����� ������ ������������ � �� ������ ��������
    double distance_to_parent // ���������� �� ������ ������������ �����
    ) ;
  
   int makeRoot( const PointType& pt )  {   return makeSphere( 0, pt, -1, 0 );  }

  int insertPoint( 
    // ���������� ������������� ����� ����� �� ������ ������, � ������� ����������� ��� ������� ���� �������
    const PointType& pt, // ����� ������ ����� iSphere �� ������ iSphereLevel
    int iSphere, 
    int iSphereLevel, 
    double dist // ���������� �� ������ ����� iSphere (��� ��������)
    );


  int attachPoint( const PointType& pt, int iSphere, int iSphereLevel, double dist );
    // ����� ��������������� � ������ �����, �� ������� ��������� ����
 
  // ---- ������� � ��������� ----
public:  
  double computeDistance( int iSphere,  const PointType& pt );
 
  int getSpheresCount() { return int( spheres.size() ); };
  const CoverSphere< PointType >& getSphere( int iSphere ) { return spheres[iSphere]; };
  int countKids( int iSphere ) ;// ������������ ���������� ���������������� ����� � �������

public:
  const PointType& 
  findNearestPoint( // ��������� � ��������� ����� ����� ����� �� ������
    const PointType& pt, // ����� ��� ������� ���� ����� � ��������� �������
    double &best_distance,// [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������)
    int iStartSphere = 0// � ����� ����� �������� �����, 0 - ������ ������ 
  );
public:
  bool checkCoverNetSphere(int iSphere, int iKidSphere); // �������, ����������� ������������ ���������� ������ -- �� ������ ������ ��� ����, ��� ��� ������� ������ ������ �����
  
  void countUncles(int iFrom, int iRoot = 0); // ������� ���� ������ ������ �����

  bool checkCoverNet();

  double// ���������� ������ �� k ���������� � best_spheres
  findKNearestSpheres(// ��������� � best_spheres - ���� best_spheres.size() < k, �� ���� ���� �����, ��� k
    const PointType& pt, // ����� ��� ������� ���� ����� � ���������� ��������
    int k,// ���������� ����, ������� �� ����� �����
    std::priority_queue<std::pair<double, int> > &best_spheres,// ���� ������ ���� (���� - ����������, ������), ������� � ����������� ���������
	  double distanceToPt = -1, // ���������� �� �����, -1 -- ���� �� ���������
    int iStartSphere = 0,// � ����� ����� �������� �����, 0 - ������ ������
    bool is_copy = false //� �� �������� �� ��� ����� ������ �����-��
  );
  

  public:
    std::vector<std::pair<PointType, double> >  // � ���� �� ���������� ����������? -- ���������� ���� �����, ���������� -- ������������� �� ����������� ����������
  findKNearestPoints( // ��������� � ��������� ����� ������ ���� �� ������
    const PointType& pt,// ����� ��� ������� ���� ����� � ��������� �������
    int k, // ���������� ������
    double best_distance=1e100// [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������)
  , int iStartSphere = 0// � ����� ����� �������� �����, 0 - ������ ������ 
  );

  int // ����� �����, (-1 ���� �� ��������� ������� ���������)
  findNearestSphere(
    const PointType& pt, // ����� ��� ������� ���� ����� � ��������� �������
    double& best_distance,// [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������)
	double distanceToPt = -1, // ���������� �� �����, -1 -- ���� �� ���������
    int iStartSphere = 0// � ����� ����� �������� �����, 0 - ������ ������ 
  );

  void  uploadSpheresByLevel( 
    int i_level, // requested level
    std::vector< std::pair< int , int > >& proper_spheres, std::vector<int> &trueWeight ///<.points, index>
    );

  int makeClusters( // ���������� ����� ��������� (=res_clusters.size())
    //vector< PointType >&  points, // ���� ����� �����
    //???? int sigma, // ����� ������� ��� ������� �������, �������� �������
    int   i_level, // ������������ ����� ���������� ������
    std::vector< PointsCluster< PointType> >& res_clusters, // ���� ��������� �������� { center, points } // { center, sigma }
    int   minPoints = 3,  // ������� ������ ��������� �� ������ ���������� ���-�� �����
    int   maxClusters = 100 // but not more than maxClustersCount
                     );
  
  void printNode( int node, int level );
  
  void reportStatistics( int node =0,  int detailsLevel=3 ); 

  public:
  void countAncles();// ��������� ������ ������ �����
  private:
  void countAncles_(int fromNum, int num);// fromNum - ����� ���������. num - ����� ����� ��� ������� ��������� �����

  public:
  void testAncles(); // ��������� ������

  public:
  void printKids(int num);

  public:
  void testKNearestPoints(PointType p, int k);  // ��������� k ���������
};

template < class PointType, class Metrics>
int CoverNet<PointType, Metrics>::insert( 
    // ��������� �����, ���������� ������������� ����� ����� ������� ������, � ������� ����������� ��� �������; 
    // ���������� 0 ���� ����� ����� �� ��������� ������ ����� ��������� (������� ����)
    const PointType& pt // ����������� ����� 
    )
  {
    attemptsToInsert++;

    if (spheres.size() == 0)
      return makeRoot( pt );

    double dist_root = computeDistance( 0, pt );
    if ( !(dist_root < getRadius(0)) )
    {
#ifdef COVER_NET_VERBOSE_DETAILED
      std::cout << "Point " << pt << " lies too far, distance to root == " << dist_root << std::endl;
#endif
      rejectedInserts++;
      return 0; // ���������� ������� ������� �����
    }
	return insertPoint( pt, 0, 0, dist_root ); // ��������� ��������� � ����
  }
  
  template < class PointType, class Metrics>
  void CoverNet<PointType, Metrics>::countTrueWeight(std::vector<int> &result) // ��������� ��� ������ ����� ������� ����� ��� ��������� �� ����� ����
  {
    result.assign(spheres.size(), -1);
    for (int i = 0; i < (int)spheres.size(); ++i)
    {
      //result[i] = spheres[i].points;
      result[i] = countTrueWeight_(i, getRadius(spheres[i].level), 0);
    }
  }

  template < class PointType, class Metrics>
  int CoverNet<PointType, Metrics>::countTrueWeight_(int fromNum, double fromRad, int num) // �������� ��� ���������� ����, fromNum - ����� �����, ��� ������� ��������� ���, num - ����� ������������ �����
  {
    int local_result = 0;
    int level = spheres[num].level;
    double radius = getRadius(level);
    double dist = computeDistance(fromNum, spheres[num].center);
  
    if (fromRad >= dist + radius) // ���� ����� ����� �������� ������ �������� !
    {
      local_result += spheres[num].points; 
      return local_result;
    } 
   
    if (fromRad + radius < dist) // ���� ����� ������ �� ������������ !
    {
      local_result = 0;
      return local_result;
    } 

    //���������� ������ -- ����� ������������ �� �� ����� ���� � �����
    int kid = spheres[num].last_kid;// ��������� �������
    while (kid > 0)// ���� �� ���� �����
    {
      local_result += countTrueWeight_(fromNum, fromRad, kid);
      kid = spheres[kid].prev_brother;
    }

    return local_result;
  }

  template < class PointType, class Metrics>
  void  CoverNet<PointType, Metrics>::notifyParents( const PointType& pt, int iSphere, int iSphereLevel, int eventToNote  )
  {
    switch (eventToNote) 
    {
      case SPHERE_CREATED: break;
      case POINT_ATTACHED: break;
    }
  }
	
  template < class PointType, class Metrics>
  int CoverNet<PointType, Metrics>::makeSphere( // ������� �����, �� ������ ����������� � ���������� �� �����
    int iSphereLevel, // ������� ����������� �����
    const PointType& pt, // ����� �����
    int parent, // ������������ ����� (�������������, ��� ����� ����� ������ ������������ � �� ������ ��������
    double distance_to_parent // ���������� �� ������ ������������ �����
    ) 
  {
    //assert(distance_to_parent == ( (parent >= 0) ? computeDistance( parent, pt ) : distance_to_parent ) );
    //if (iSphereLevel > 0) // not root
    //  assert( computeDistance( parent, pt ) <= getRadius(iSphereLevel-1) + 0.0000001 );


    do {
      spheres.push_back( CoverSphere<PointType>( iSphereLevel, pt, parent, distance_to_parent ) );
      iLastSphere = int( spheres.size() ) -1;
      iLastSphereLevel = iSphereLevel;
      if (parent >=0) // not root
      {
#ifdef DONT_FORCE_DIRECT_SUCCESSORS 
        int bro = spheres[parent].last_kid; // ��������� ������ ����� ������������� �� ���������� �� ��������???
        spheres[parent].last_kid = iLastSphere;
        spheres[iLastSphere].prev_brother = bro;
#else
		    int bro = spheres[parent].last_kid;//��������� �������
        if (bro == 0) // bro ����
        {
          spheres[parent].last_kid = iLastSphere;//����� iLastSphere -- ������ �������
          spheres[iLastSphere].prev_brother = bro;// = 0;
          /*if (spheres[iLastSphere].center != spheres[parent].center)
            cout << "O_O" << endl;
          else
            cout << "+" << endl;*/
        }
        else
        {
          int prev_bro = spheres[bro].prev_brother;//������������� �������
          spheres[bro].prev_brother = iLastSphere;// ������������� ������ �����
          spheres[iLastSphere].prev_brother = prev_bro;//����� ����� ����� prev_bro
        }
#endif
      }
#ifdef DONT_FORCE_DIRECT_SUCCESSORS
      break;
#endif
      if ( getRadius(iSphereLevel) <= getMinRadius(iSphereLevel) ) // ������ /// while ( getRadius(iSphereLevel) > getMinRadius(iSphereLevel) );
        break;
      iSphereLevel++;                                               // ��������� ��� ��� ��� �� ��������� �� ��������� �������
      parent = iLastSphere;
      distance_to_parent = 0;
    } while (1); /// ( getRadius(iSphereLevel) > getMinRadius(iSphereLevel) );

    notifyParents( pt, parent, iSphereLevel, SPHERE_CREATED );
    return iLastSphere;
  }

  template < class PointType, class Metrics>
  int CoverNet<PointType, Metrics>::insertPoint( 
    // ���������� ������������� ����� ����� �� ������ ������, � ������� ����������� ��� ������� ���� �������
    const PointType& pt, // ����� ������ ����� iSphere �� ������ iSphereLevel
    int iSphere, 
    int iSphereLevel, 
    double dist // ���������� �� ������ ����� iSphere (��� ��������)
    )
  { 
    // �� ������ �����
    //assert( computeDistance( iSphere, pt ) <= getRadius(iSphereLevel) + 0.0000001 );
    spheres[iSphere].points++;

    double minrad = getMinRadius(iSphereLevel);
    if (dist < minrad)
    { // ����� ��������������� � ������ �����
      return attachPoint( pt, iSphere, iSphereLevel, dist ); // ����� ��������������� � ������ ����� � �� ������ �����������
    }

    // ������ ��� �� �������
    int kid = spheres[iSphere].last_kid;
    double rlast_kid = getRadius(iSphereLevel+1);

    int best_kid = -1;
    double best_dist = rlast_kid;

    while (kid > 0)
    {
      double dist_kid = computeDistance( kid, pt );
      if (dist_kid < best_dist)
      {
        best_kid = kid;
        best_dist = dist_kid;
        //
        //return; // ����� �������
      }
      kid = spheres[kid].prev_brother;
    }


    int isp=0;
    if (best_kid != -1)
      isp = insertPoint( pt, best_kid, iSphereLevel+1, best_dist ); // �����������
    else // ��� �����, ����� ��������, ������� ����� �, ��������, �� ������ �����������
      isp = makeSphere( iSphereLevel+1,  pt, iSphere, dist );

    return isp;
  }
	
   template < class PointType, class Metrics>	
   int CoverNet<PointType, Metrics>::attachPoint( const PointType& pt, int iSphere, int iSphereLevel, double dist )
    // ����� ��������������� � ������ �����, �� ������� ��������� ����
  {
#ifdef COVER_NET_VERBOSE_DETAILED
    std::cout << "Point " << pt << " lies near to sphere " << spheres[iSphere].center << " distance == " << dist << std::endl;
#endif
    iLastSphere = iSphere;
    iLastSphereLevel = iSphereLevel;
    // add weight?
    // average radii? other node statistics?
    //spheres[iSphere].sumradii += dist;
    //spheres[iSphere].sumradiisq += dist*dist;

    /////---- ����������� ������� �����, ����������� ������ ������ � � ������ ����������� ���� (note!)
    int isp = spheres[iSphere].last_kid;
    while (isp)
    {
      assert( spheres[isp].center == spheres[ spheres[isp].parent ].center );
      spheres[isp].points++;
      iSphere = isp;
      isp = spheres[isp].last_kid;
    }
    ////----

    notifyParents( pt, iSphere, iSphereLevel, POINT_ATTACHED );
    return iSphere;
  }
	
  template < class PointType, class Metrics>		
  double CoverNet<PointType, Metrics>::computeDistance( int iSphere,  const PointType& pt )
  {
    double dist = ruler->computeDistance( spheres[iSphere].center, pt );
    return dist;
  }	

  template < class PointType, class Metrics>		
  int CoverNet<PointType, Metrics>::countKids( int iSphere ) // ������������ ���������� ���������������� ����� � �������
  { 
    int count=0;
    for (int kid = spheres[iSphere].last_kid; kid > 0; kid = spheres[kid].prev_brother)
      count++;
    return count; 
  }
  

  template < class PointType, class Metrics> 
  const PointType& 
  CoverNet<PointType, Metrics>::findNearestPoint( // ��������� � ��������� ����� ����� ����� �� ������
    const PointType& pt, // ����� ��� ������� ���� ����� � ��������� �������
    double &best_distance,// [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������)
    int iStartSphere/* = 0 */ // � ����� ����� �������� �����, 0 - ������ ������  // redeclaration, no default params!
  )
  {
    using namespace std;
    int iNearestSphere = findNearestSphere(pt, best_distance, -1, iStartSphere);
	  if (iNearestSphere == -1)
      cerr << "error: distance to root is more then maximal radius" << endl;
	  return spheres[iNearestSphere].center;
  }
  
  template < class PointType, class Metrics> 
  bool CoverNet<PointType, Metrics>::checkCoverNetSphere(int iSphere, int iKidSphere) // �������, ����������� ������������ ���������� ������ -- �� ������ ������ ��� ����, ��� ��� ������� ������ ������ �����
  { 
      using namespace std;
      int isp=iSphere; // ������� �����
	    int lev = spheres[isp].level; // ������� �������
      double  rad = getRadius(lev);
      double dist = computeDistance(isp, spheres[iKidSphere].center);
      if (dist > rad)// ���� ���������� �� ������� ������ �������
      {
         cout << "build tree error" << endl;
         cout << "incorrect distance " << dist << " from sphere N = " << iSphere 
           << " with center in " << spheres[isp].center << " and R = " << rad << " to kid N = " << iKidSphere 
           << " with center in " << spheres[iKidSphere].center << endl;
         system ("pause");
         return false;
      }
      for (int kid = spheres[iKidSphere].last_kid; kid > 0; kid = spheres[kid].prev_brother)// ���� �� ���� �����
	    {
		    int kid_ans = checkCoverNetSphere(iSphere, kid);
        if (!kid_ans)
          return false;
      }

      return true;    
  }
  
  template < class PointType, class Metrics> 
  bool CoverNet<PointType, Metrics>::checkCoverNet()
  {
     for (int i = 0; i < spheres.size(); ++i)
         if (!checkCoverNetSphere(i, i))
           return false;
     return true;
  }
  
  template < class PointType, class Metrics> 
   double// ���������� ������ �� k ���������� � best_spheres
  CoverNet<PointType, Metrics>::findKNearestSpheres(// ��������� � best_spheres - ���� best_spheres.size() < k, �� ���� ���� �����, ��� k
    const PointType& pt, // ����� ��� ������� ���� ����� � ���������� ��������
    int k,// ���������� ����, ������� �� ����� �����
    std::priority_queue<std::pair<double, int> > &best_spheres,// ���� ������ ���� (���� - ����������, ������), ������� � ����������� ���������
    double distanceToPt/* = -1*/, // ���������� �� �����, -1 -- ���� �� ��������� // redeclaration, no default params!
    int iStartSphere/* = 0*/,// � ����� ����� �������� �����, 0 - ������ ������   // redeclaration, no default params!
    bool is_copy/* = false*/ //� �� �������� �� ��� ����� ������ �����-��         // redeclaration, no default params!
  )
  {
    using namespace std;
    int isp=iStartSphere; // ������� �����
	  int lev = spheres[isp].level; // ������� �������
    double  rad = getRadius(lev);// ������ ������� ����� (�� ������ ������)
    double dist = distanceToPt;

	  if (dist == -1) // ���� ���������� ��� �� ���������
		  dist = computeDistance(isp, pt);
	 
    const double INF = 1e100; //�������������
	  if (isp == 0 && dist >= rad)// ���� �� ��������� ���������
		  return INF;

    double min_dist = std::max(0.0, dist - rad);// ���������� �� pt �� �����
    
   
    double best_kth_distance = INF;
    if (best_spheres.size() == k)
    {
        pair<double, int>  kth_sphere = best_spheres.top();
        best_kth_distance = kth_sphere.first;
    }


	  if (min_dist > best_kth_distance)// ���� ����������� ���������� ������ ������������
		  return best_kth_distance;
	  if (dist < best_kth_distance && !is_copy)// ���� ����� �������� ����� (���� �� ����� ������ �����)//!!!!!!!!!!!
	  {
		   best_spheres.push(make_pair(dist, isp)); // ��������� ����� ����������
       if (int( best_spheres.size()) > k) // ���� ���� ������ ��� k
       {
          best_spheres.pop();
       }
       best_kth_distance = dist;
	  }

	  const int MAX_KIDS_SIZE = 1000;
	  pair<double, int> kids[MAX_KIDS_SIZE];

	  if (spheres[isp].last_kid == 0) // ����
		  return best_kth_distance;

    int num = -1;
	  int kid = spheres[isp].last_kid;
	  int kids_size = 0;
	  if (spheres[kid].center == spheres[isp].center)
	  {
		  kids[kids_size++] = make_pair(dist, kid);
      num = kid;
		  kid = spheres[kid].prev_brother;
	  }

	  for (; kid > 0; kid = spheres[kid].prev_brother)// ���� �� ���� �����
    {
      if (fabs(spheres[kid].distance_to_parent - dist) < best_kth_distance + getRadius(lev + 1)) 
		    kids[kids_size++] = make_pair(computeDistance(kid, pt), kid);
    }
	  
	  sort(kids + 0, kids + kids_size);
	  for (int i = 0; i < kids_size; ++i)
	  {
      if (best_kth_distance + getRadius(lev + 1) <= fabs(spheres[kids[i].second].distance_to_parent - dist))// ���� �� ����������� ������������, ������ �������� �����
       continue;
      if (num == kids[i].second)
        best_kth_distance = min(best_kth_distance, findKNearestSpheres(pt, k, best_spheres, kids[i].first, kids[i].second, true));
      else
		    best_kth_distance = min(best_kth_distance, findKNearestSpheres(pt, k, best_spheres, kids[i].first, kids[i].second, false));
    }
    return best_kth_distance;
  }

  template < class PointType, class Metrics> 
    std::vector<std::pair<PointType, double> >  // � ���� �� ���������� ����������? -- ���������� ���� �����, ���������� -- ������������� �� ����������� ����������
  CoverNet<PointType, Metrics>::findKNearestPoints( // ��������� � ��������� ����� ������ ���� �� ������
    const PointType& pt,// ����� ��� ������� ���� ����� � ��������� �������
    int k, // ���������� ������
    double best_distance/* = 1e100*/ // [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������) //redeclaration, no default params!
  , int iStartSphere/* = 0*/// � ����� ����� �������� �����, 0 - ������ ������                                          //redeclaration, no default params!
  )
  {
    using namespace std;
    priority_queue<pair<double, int> > best_dist;
    for (int i = 0; i < k; ++i)
      best_dist.push(make_pair(best_distance, -1)); // ��������� ��������� ������� � ����������� best_distance
    
    findKNearestSpheres(pt, k, best_dist, -1, iStartSphere); // ������ �����;
    
    vector<pair<PointType, double> > result;
    while (!best_dist.empty())
    {
       pair<double, int> pt = best_dist.top();
       best_dist.pop();
       if (pt.second == -1)
         continue;

       result.push_back(make_pair(spheres[pt.second].center, pt.first));
    }

    reverse(result.begin(), result.end());
    return result;
  }
  
   template < class PointType, class Metrics> 
   int // ����� �����, (-1 ���� �� ��������� ������� ���������)
   CoverNet<PointType, Metrics>::findNearestSphere(
    const PointType& pt, // ����� ��� ������� ���� ����� � ��������� �������
    double& best_distance,// [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������)
  double distanceToPt/* = -1*/, // ���������� �� �����, -1 -- ���� �� ��������� // redeclaration, no default params!
    int iStartSphere/* = 0*/ // � ����� ����� �������� �����, 0 - ������ ������ // redeclaration, no default params!
  )
  {
    using namespace std;
    if (spheres.empty())
      return -1;
    int isp=iStartSphere; // ������� �����
	  int lev = spheres[isp].level; // ������� �������
    double  rad = getRadius(lev);// ������ ������� ����� (�� ������ ������)
    double dist = distanceToPt;

	if (dist == -1)
		dist = computeDistance(isp, pt);
	
	if (isp == 0 && dist >= rad)// ���� �� ��������� ���������
		return -1;

	  int ans = -1;
    double min_dist = std::max(0.0, dist - rad);// ���������� �� pt �� �����
	  if (min_dist > best_distance)// ���� ����������� ���������� ������ ������������
		  return -1;
	  if (dist < best_distance)// ���� ����� �������� �����
	  {
		  ans = isp;
		  best_distance = dist;
	  }

	  const int MAX_KIDS_SIZE = 1000;  // <<<<<<<< TODO --- CHECK!!!
	  pair<double, int> kids[MAX_KIDS_SIZE];

	  if (spheres[isp].last_kid == 0) // ����
		  return ans;

	  int kid = spheres[isp].last_kid;
	  int kids_size = 0;
	  if (spheres[kid].center == spheres[isp].center)
	  {
		  kids[kids_size++] = make_pair(dist, kid);
		  kid = spheres[kid].prev_brother;
	  }

	  for (; kid > 0; kid = spheres[kid].prev_brother)// ���� �� ���� �����
    {
      if (fabs(spheres[kid].distance_to_parent - dist) < best_distance + getRadius(lev + 1)) // ���� �� ����������� ������������, ����� �������� �����
		   kids[kids_size++] = make_pair(computeDistance(kid, pt), kid);  // <<<<<<<<<< CHECK kids_size++
    }
	  
	  sort(kids + 0, kids + kids_size);
	  for (int i = 0; i < kids_size; ++i)
	  {
      if (best_distance + getRadius(lev + 1) <= fabs(spheres[kids[i].second].distance_to_parent - dist))// ���� �� ����������� ������������, ������ �������� �����
       continue;
		  int new_ans = findNearestSphere(pt, best_distance, kids[i].first, kids[i].second);
		  if (new_ans != -1)
			  ans = new_ans;
	  }

	  return ans;
  }
  
  template < class PointType, class Metrics> 
  void   CoverNet<PointType, Metrics>::uploadSpheresByLevel( 
    int i_level, // requested level
    std::vector< std::pair< int , int > >& proper_spheres, std::vector<int> &trueWeight ///<.points, index>
    )
  {
    using namespace std;

    for (int i=0; i < getSpheresCount(); i++)
    {
      const CoverSphere< PointType  > & s = getSphere( i );
      if (s.level != i_level) 
        continue;
      proper_spheres.push_back( make_pair( trueWeight[i], i ) );
    }

    sort( proper_spheres.rbegin(), proper_spheres.rend() );
  }

  template < class PointType, class Metrics> 
  int CoverNet<PointType, Metrics>::makeClusters( // ���������� ����� ��������� (=res_clusters.size())
    //vector< PointType >&  points, // ���� ����� �����
    //???? int sigma, // ����� ������� ��� ������� �������, �������� �������
    int   i_level, // ������������ ����� ���������� ������
    std::vector< PointsCluster< PointType> >& res_clusters, // ���� ��������� �������� { center, points } // { center, sigma }
    int   minPoints/* = 3*/,  // ������� ������ ��������� �� ������ ���������� ���-�� �����   // redeclaration, no default params!
    int   maxClusters/* = 100*/ // but not more than maxClustersCount                         // redeclaration, no default params!
                     )
  {
    using namespace std;
    vector< pair< int , int > > proper_spheres; ///<points, index>
    vector<int> W;
    countTrueWeight(W);
    uploadSpheresByLevel( i_level, proper_spheres, W);
    // upload to res_clusters
    res_clusters.clear();

    for (int i=0; i< int( proper_spheres.size() ); i++)
    {
      if (proper_spheres[i].first < minPoints)
        break; // ���� ����� � ��������
      if (int(res_clusters.size()) >= maxClusters)
        break; // �������, ������� ����� ���������
      int isphere = proper_spheres[i].second; // ������������� �� ��������, � ��� ����� �����
      const CoverSphere< PointType  > & s = getSphere( isphere );
      //MyPoint center = s.center;
      //Point xycenter = points[center];
      double rad = getRadius( s.level );
      PointsCluster<PointType> cluster( s.center, rad, W[proper_spheres[i].second] );
    //  cout << W[proper_spheres[i].second] << endl;;
      res_clusters.push_back( cluster );
    }
   // cout << endl;
    return res_clusters.size();
  }

  template < class PointType, class Metrics> 
   void CoverNet<PointType, Metrics>::printNode( int node, int level )
  {
    spheres[node].print(level);
    for ( int kid = spheres[node].last_kid; kid > 0; kid = spheres[kid].prev_brother )
    {
          printNode( kid, level+1 );
    }
  }
  
   template < class PointType, class Metrics> 
  void CoverNet<PointType, Metrics>::reportStatistics( int node/* =0*/,  int detailsLevel/*=3*/ ) // redeclaration, no default params!
    // 0-none, 1-overall statistics, 2-by levels, 3-print tree hierarchy, 4-print tree array with links
  {
    if (detailsLevel < 1)
      return; // set breakpoint here for details

    int maxLevel=-1; // ��� �� �������
    for (int i=0; i< int( spheres.size() ); i++)
      maxLevel = std::max( maxLevel, spheres[i].level );

    std::cout 
      << "********** CoverNet: " << std::endl 
      << "spheres=" << spheres.size() 
      << "\tlevels=" << maxLevel+1 
      << "\tattemptsToInsert=" << attemptsToInsert // ������� ����� ������� �������
      << "\trejectedInserts=" << rejectedInserts // ������� ����� ������� ������� ������� ������� �����
      << std::endl;

    if (detailsLevel < 2 || maxLevel < 0)
      return; 

    if (detailsLevel < 3 || maxLevel < 0)
      return; 

    std::cout 
      << "---------- " << std::endl 
      << "Statistics by level:" << std::endl;
    std::vector< int > spheresByLevel(maxLevel+1);
    std::vector< int > pointsByLevel(maxLevel+1);
    std::vector< int > kidsByLevel(maxLevel+1);

    for (int i=0; i< int(spheres.size()); i++)
    {
      spheresByLevel[ spheres[i].level ]++;
      pointsByLevel[  spheres[i].level ]+=spheres[i].points;
      kidsByLevel[  spheres[i].level ] += countKids(i);
    }

    for (int i=0; i<=maxLevel; i++)
    {
      printf("\ntree level=%.2d radius=%8.4f spheres=%6.d points=%6.d ave_points=%10.3f kids=%5.d ave_kids=%8.3f",
        i // level
        ,getRadius(i)
        ,spheresByLevel[i] 
        ,pointsByLevel[i]  
        ,double( pointsByLevel[i])/ spheresByLevel[i]
        ,kidsByLevel[i]
        ,double( kidsByLevel[i])/ spheresByLevel[i]
        );
      //std::cout 
      //  //<< fixed 
      //  << setprecision(10)
      //  << "tree level=" << i << "\t" 
      //  << "\tradius=" << getRadius(i) 
      //  << "\tspheres=" << spheresByLevel[i] 
      //  << "\tpoints="  << pointsByLevel[i]  
      //  << "\tave=" 
      //  << setprecision(10)
      //  << double( pointsByLevel[i])/ spheresByLevel[i]
      //  << setprecision(10)
      //  << "\tsum kids="    << kidsByLevel[i]    
      //  << "\tave=" << double( kidsByLevel[i])/ spheresByLevel[i]
      //  << std::endl;
    }

    if (detailsLevel < 4)
      return; 

    std::cout 
      << "---------- " << std::endl 
      << "Spheres heirarchy:" << std::endl;
    printNode(0, 0);
    std::cout << "********** " << std::endl;

    if (detailsLevel < 4)
    for (int i=0; i< int(spheres.size()); i++)
      spheres[i].print( 0 );

  }
  

  template < class PointType, class Metrics>
  void CoverNet<PointType, Metrics>::countAncles() // ��������� ������ ������ �����
  {
    ancles.resize(0);
    for (int i = 1; i < (int)spheres.size(); ++i)
    {
      spheres[i].ancle = -1;
      countAncles_(0, i);
    }
  }

  template < class PointType, class Metrics>
  void CoverNet<PointType, Metrics>::countAncles_(int fromNum, int num) // fromNum - ����� ���������. num - ����� ����� ��� ������� ��������� �����
  {
    using namespace std;
    int level = spheres[fromNum].level;
    double radius = getRadius(level);
    double dist = computeDistance(fromNum, spheres[num].center);
  
    if (level + 1 == spheres[num].level) // ���� ����� �� ������� ���� - �� ���� ����� ���� �����
    {
      if (radius >= dist) // ���� ��� ��������� �����
      {
        ancles.push_back(make_pair(fromNum, spheres[num].ancle));
        spheres[num].ancle = ancles.size() - 1;
      }
      return;// ���� ���������� �����������
    } 
   
    double add_radius = getRadius(spheres[num].level - 1); // ������ ����
    if (radius + add_radius < dist) // ���� �� ���� ����� ������ ����� fromNum �� ����� ������� num
    {
      if (num == 43)
      {
        cout << " " << fromNum << " " << spheres[num].level - 1 << " " << radius + add_radius << " " << dist << endl;
       // cout << "atata" << endl;
      }
      return;// �� ���
    } 

    
    int kid = spheres[fromNum].last_kid;// ��������� �������
    while (kid > 0)// ���� �� ���� �����
    {
      countAncles_(kid, num);
      //if (radius < computeDistance(fromNum, spheres[kid].center))
      //    cout << "Oo" << endl;
      kid = spheres[kid].prev_brother;
    }
  }

  template < class PointType, class Metrics>
  void CoverNet<PointType, Metrics>::printKids(int num)
  {
    using namespace std;
    cout << "kids of vertex num " << num << ":  ";
    int kid = spheres[num].last_kid;// ��������� �������
    while (kid > 0)// ���� �� ���� �����
    {
      cerr << kid <<" ";
      kid = spheres[kid].prev_brother;
    }
    cout << endl;
  }

  template < class PointType, class Metrics>
  void CoverNet<PointType, Metrics>::testAncles() // ��������� ������
  {
    using namespace std;
    std::cerr << "----------------------------" << std::endl;
    std::cerr << "Begin testing ancles" << std::endl;
    countAncles();

    printKids(57);
    cout  << "radius 43 ancle: "<< getRadius(spheres[43].level - 1) << endl; // ������ ���� 42 �����
    cout << "radius 57: " << getRadius(spheres[57].level) << endl; //������ 57 �����
    cout << "radius 103: " << getRadius(spheres[103].level) << endl;
    cout << "distance 43, 57: "<< computeDistance(43, spheres[57].center) << endl;
    cout << "distance 43, 103: "<< computeDistance(43, spheres[103].center) << endl;
    cout << "distance 57, 103: " << computeDistance(57, spheres[103].center) << endl;


    cout << spheres[43].center << " " << spheres[57].center << " " << spheres[103].center << endl;
    int count_error = 0;
    int sum_count = 0;
    for (int i = 1; i < spheres.size(); ++i)
    {
      std::vector<int> test_ancle;
      std::vector<int> res_ancle;
      for (int i1 = 0; i1 < spheres.size(); ++i1)
      {
        if (spheres[i].level ==  spheres[i1].level + 1) // ���� i1 ����� ���� ����� i
        {
            double rad1 = getRadius(spheres[i1].level);
            if (computeDistance(i, spheres[i1].center) <= rad1 + 1e-5)// ���� i1 ��������� i
              test_ancle.push_back(i1);// �� ����
        }
      }
      int ancl = spheres[i].ancle;// ��������� ����
      while (ancl >= 0)// ���� �� ���� �����
      {  
        res_ancle.push_back(ancles[ancl].first);
        ancl = ancles[ancl].second;
      }
      
      std::sort(test_ancle.begin(), test_ancle.end());
      std::sort(res_ancle.begin(), res_ancle.end());
      

      sum_count += test_ancle.size();
      if (test_ancle != res_ancle)
      {
        count_error += test_ancle.size() - res_ancle.size();
        /* double rad1 = getRadius(spheres[i].level - 1);
         cout << rad1 << endl;
        cout << "test_ancle_dist:  ";
        for (int j = 0; j < test_ancle.size(); ++j)
        {
          cout << computeDistance(i, spheres[test_ancle[j]].center) << " ";
        }
        cout << endl;

        cout << "test_ancle:  ";
        for (int j = 0; j < test_ancle.size(); ++j)
        {
          cout << test_ancle[j] << " ";
        }
        cout << endl;

        cout << "res_ancle:   ";
        for (int j = 0; j < res_ancle.size(); ++j)
        {
          cout << res_ancle[j] << " ";
        }
        cout << endl;
        std::cerr << "Error at sphehere number " << i << std::endl;
        system ("pause");*/



      }
    }

    std::cerr << "Sum count of ancles: " << sum_count <<std::endl; 
    std::cerr << "Errors: " << count_error << std::endl;
    std::cerr << "Errors %:" << 1.0 * count_error / sum_count << std::endl;
    std::cerr << "End testing ancles" << std::endl;
    std::cerr << "----------------------------" << std::endl;
    system ("pause");
    
  }

  template < class PointType, class Metrics>
  void CoverNet<PointType, Metrics>::testKNearestPoints(PointType p, int k) // ��������� k ���������
  {
    using namespace std;
    std::cerr << "----------------------------" << std::endl;
    std::cerr << "Begin testing k nearest points" << std::endl;

    std::vector<std::pair<PointType, double> > result = findKNearestPoints(p, k);
    std::vector<std::pair<PointType, double> > test_result;
   
    std::vector<std::pair<double, PointType> > dst;
    vector<bool> used(spheres.size(), false);
    for (int i = 0; i < spheres.size(); ++i)
    {
      if (used[spheres[i].center])
        continue;
      used[spheres[i].center] = true;
      dst.push_back(make_pair(computeDistance(i, p), spheres[i].center));
    }
    sort(dst.begin(), dst.end());
    for (int i = 0; i < min((int)spheres.size(), k); ++i) 
    {
      test_result.push_back(make_pair(dst[i].second, dst[i].first));
    }
   
    bool ch = true;
    for (int i = 0; i < min((int)spheres.size(), k); ++i)
    {
      if (fabs(test_result[i].second - result[i].second) > 1e-3)
        ch = false;
    }
    if (!ch)
    {
      cout << "result: ";
      cout.precision(3);
      for (int i = 0; i < result.size(); ++i)
      {
        cout << fixed << result[i].second << " ";
      }
      cout << endl;
      cout << "test  : ";
      cout.precision(3);
      for (int i = 0; i < test_result.size(); ++i)
      {
        cout << fixed << test_result[i].second << " ";
      }
      cout << endl;

      cout << "ERROR" << endl;
    }

    std::cerr << "End testing k nearest points" << std::endl;
    std::cerr << "----------------------------" << std::endl;
    system ("pause");
    
  }


#endif // __COVER_NET_H


