// cover_net.h
//#ifdef false

#ifndef __COVER_NET_H
#define __COVER_NET_H

#include <vector>
#include <algorithm>
#include <cassert>
#include <queue>
#include <iomanip>

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
    string indent;
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
  std::vector< std::pair< int, int > > ancles; // <ancle sphere, next> ������ ������ ������� ����, ������� ��������� ������ 

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
    double best_distance// [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������)
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
      iLastSphere = spheres.size()-1;
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
    int iStartSphere = 0// � ����� ����� �������� �����, 0 - ������ ������ 
  )
  {
    int iNearestSphere = findNearestSphere(pt, best_distance, -1, iStartSphere);
	  if (iNearestSphere == -1)
		  cerr << "error: distance to root is more then maximal radius" << endl;
	  return spheres[iNearestSphere].center;
  }
  
  template < class PointType, class Metrics> 
  bool CoverNet<PointType, Metrics>::checkCoverNetSphere(int iSphere, int iKidSphere) // �������, ����������� ������������ ���������� ������ -- �� ������ ������ ��� ����, ��� ��� ������� ������ ������ �����
  { 
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
	  double distanceToPt = -1, // ���������� �� �����, -1 -- ���� �� ���������
    int iStartSphere = 0,// � ����� ����� �������� �����, 0 - ������ ������
    bool is_copy = false //� �� �������� �� ��� ����� ������ �����-��
  )
  {
    int isp=iStartSphere; // ������� �����
	  int lev = spheres[isp].level; // ������� �������
    double  rad = getRadius(lev);// ������ ������� ����� (�� ������ ������)
    double dist = distanceToPt;

	  if (dist == -1) // ���� ���������� ��� �� ���������
		  dist = computeDistance(isp, pt);
	 
    const double INF = 1e100; //�������������
	  if (isp == 0 && dist >= rad)// ���� �� ��������� ���������
		  return INF;

	  double min_dist = max(0.0, dist - rad);// ���������� �� pt �� �����
    
   
    double best_kth_distance = INF;
    if (best_spheres.size() == k)
    {
        pair<double, int>  kth_sphere = best_spheres.top();
        best_kth_distance = kth_sphere.first;
    }


	  if (min_dist > best_kth_distance)// ���� ����������� ���������� ������ ������������
		  return best_kth_distance;
	  if (dist < best_kth_distance && !is_copy)// ���� ����� �������� ����� (���� �� ����� ������ �����)
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
    double best_distance// [in/out] ������ ���������� -- ��� �� � ��������� (�� ������ ������ ��� �������)
  , int iStartSphere = 0// � ����� ����� �������� �����, 0 - ������ ������ 
  )
  {
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
	double distanceToPt = -1, // ���������� �� �����, -1 -- ���� �� ���������
    int iStartSphere = 0// � ����� ����� �������� �����, 0 - ������ ������ 
  )
  {
    int isp=iStartSphere; // ������� �����
	int lev = spheres[isp].level; // ������� �������
    double  rad = getRadius(lev);// ������ ������� ����� (�� ������ ������)
    double dist = distanceToPt;

	if (dist == -1)
		dist = computeDistance(isp, pt);
	
	if (isp == 0 && dist >= rad)// ���� �� ��������� ���������
		return -1;

	  int ans = -1;
	  double min_dist = max(0.0, dist - rad);// ���������� �� pt �� �����  
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
    int   minPoints = 3,  // ������� ������ ��������� �� ������ ���������� ���-�� �����
    int   maxClusters = 100 // but not more than maxClustersCount
                     )
  {
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
  void CoverNet<PointType, Metrics>::reportStatistics( int node =0,  int detailsLevel=3 ) 
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
      std::cout 
        //<< fixed 
        << setprecision(5)
        << "tree level=" << i << "\t" 
        << "\tradius=" << getRadius(i) 
        << "\tspheres=" << spheresByLevel[i] 
        << "\tpoints="  << pointsByLevel[i]  << "\tave=" << double( pointsByLevel[i])/ spheresByLevel[i]
        << "\tsum kids="    << kidsByLevel[i]    << "\tave=" << double( kidsByLevel[i])/ spheresByLevel[i]
        << std::endl;
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
  
#endif // __COVER_NET_H


