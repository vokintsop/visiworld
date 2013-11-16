// cover_tree.h

#ifndef __COVER_TREE_H
#define __COVER_TREE_H

#include <vector>

#define COVER_TREE_VERBOSE
//#define COVER_TREE_VERBOSE_DETAILED

template < class PointType >
struct CoverSphere
{
  int parent;    // �������� // fat -- �� ����������� ��� �������� ��������
  int prev_brother;   // ������ ����������� �����; 0 -- ���
  int last_kid;  // ��������� �� ���������� ���������� �������; 0 -- ���

  PointType center;
  int points;   // ���������� �����, ��������������� � ������ �������� // fat
  int level;    // ������� ������� � ������ // fat

  CoverSphere( 
    int parent, 
    const PointType& center,
    int level
    ):
    parent(parent),
    center(center), 
    level(level),
    prev_brother(0),
    last_kid(0),
    points(0)
  {
  } 

  void print(int level)
  {
    string indent;
    for (int i=0; i<level; i++)
      indent.push_back('\t');

    std::cout << indent << center;
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
class CoverTree
{
  std::vector< CoverSphere< PointType > > spheres;
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


  CoverTree( Metrics* ruler, double rootRadius, double minRadius, 
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

  bool insert( const PointType& pt ) // false -- ����� �� ��������� ������ ����� ��������� (������� ����)
  {
    attemptsToInsert++;

    if (spheres.size() == 0)
    {
      makeRoot( pt );
      spheres[0].points++;
      return true;
    }

    double dist_root = computeDistance( 0, pt );
    if ( !(dist_root < getRadius(0)) )
    {
#ifdef COVER_TREE_VERBOSE_DETAILED
      std::cout << "Point " << pt << " lies too far, distance to root == " << dist_root << std::endl;
#endif
      rejectedInserts++;
      return false; // ���������� ������� ������� �����
    }

#ifndef NO_SEQUENTAL_PROXIMITY_ASSUMPTION  // ���� ������������ ������� ������, �� ������ �������� ������
    if (iLastSphere != -1)
    {
      double dist = computeDistance( iLastSphere, pt );
      double rad = getRadius(iLastSphereLevel);
      if ( dist < rad )
      {
        int start=iLastSphere;
        insertPoint( pt, iLastSphere, iLastSphereLevel, dist ); // �������� iLastSphere & iLastSphereLevel
        for (int isp = spheres[start].parent; isp >=0; isp = spheres[isp].parent)
          spheres[isp].points++;
        return true;
      }
    }
#endif

    insertPoint( pt, 0, 0, dist_root ); // ��������� ��������� � ����

    return true;
  }

private:
  enum { SPHERE_CREATED, POINT_ATTACHED };
  void  notifyParents( const PointType& pt, int iSphere, int iSphereLevel, int eventToNote  )
  {
    switch (eventToNote) 
    {
      case SPHERE_CREATED: break;
      case POINT_ATTACHED: break;
    }
  }

  int makeSphere( const PointType& pt, int parent, int iSphereLevel ) // ������� ����� � ���������� �� �����
  {
    spheres.push_back( CoverSphere<PointType>( parent, pt, iSphereLevel ) );
    iLastSphere = spheres.size()-1;
    iLastSphereLevel = iSphereLevel;
    notifyParents( pt, parent, iSphereLevel, SPHERE_CREATED );
    return iLastSphere;
  }

  void makeRoot( const PointType& pt )  {   makeSphere( pt, -1, 0 );  }

  void insertPoint( const PointType& pt, int iSphere, int iSphereLevel, double dist )
  { // �� ������ �����
    spheres[iSphere].points++;

    double minrad = getMinRadius(iSphereLevel);
    if (dist < minrad)
    { // ����� ��������������� � ������ �����
      attachPoint( pt, iSphere, iSphereLevel, dist ); // ����� ��������������� � ������ �����, �� ������� ��������� ����
      return;
    }

    // ������ ��� �� �������
    int kid = spheres[iSphere].last_kid;
    double rlast_kid = getRadius(iSphereLevel+1);
    while (kid > 0)
    {
      double dist = computeDistance( kid, pt );
      if (dist < rlast_kid)
      {
        insertPoint( pt, kid, iSphereLevel+1, dist );
        return;
      }
      kid = spheres[kid].prev_brother;
    }

    // ������� �����
    int isp = makeSphere( pt, iSphere, iSphereLevel+1 );
    int bro = spheres[iSphere].last_kid;
    spheres[iSphere].last_kid = isp;
    spheres[isp].prev_brother = bro;
  }

  void attachPoint( const PointType& pt, int iSphere, int iSphereLevel, double dist )
    // ����� ��������������� � ������ �����, �� ������� ��������� ����
  {
#ifdef COVER_TREE_VERBOSE_DETAILED
    std::cout << "Point " << pt << " lies near to sphere " << spheres[iSphere].center << " distance == " << dist << std::endl;
#endif
    iLastSphere = iSphere;
    iLastSphereLevel = iSphereLevel;
    // add weight?
    // average radii? other node statistics?
    //spheres[iSphere].sumradii += dist;
    //spheres[iSphere].sumradiisq += dist*dist;

    notifyParents( pt, iSphere, iSphereLevel, POINT_ATTACHED );
  }

  // ---- ������� � ��������� ----
public:  
  double computeDistance( int iSphere,  const PointType& pt )
  {
    double dist = ruler->computeDistance( spheres[iSphere].center, pt );
    return dist;
  }

  int getSpheresCount() { return int( spheres.size() ); };
  const CoverSphere< PointType >& getSphere( int iSphere ) { return spheres[iSphere]; };
  int countKids( int iSphere ) // ������������ ���������� ���������������� ����� � �������
  { 
    int count=0;
    for (int kid = spheres[iSphere].last_kid; kid > 0; kid = spheres[kid].prev_brother)
      count++;
    return count; 
  };

  int // ����� �����, (-1 ���� �� ��������� ������� ���������), ���������� ��������        
  branchSubTreeUsingFirstCover(   // ������������� ����� �� ��������� -- ������ ���� branch&bounds
    const PointType& pt, // ����� ��� ������� ���� ����� �� ����� -- ���� ��������� -- ����� ���������� 
                         // �.�. ��� ������� ������ ����� ������� �� ��� �� ������ 
    int iStartSphere = 0,// � ����� ����� �������� �����, 0 - ������ ������ 
    int iStartLevel = 0 // ������� ��������� �����, 0 - ������� ������ ������ 
//    ,CoverRecord& record // ������������ ������ ����������?
    ) 
  {
    double dist = computeDistance( iStartSphere, pt );
//  record.update( dist, iStartSphere, iStartLevel );
    if (!(dist < getRadius( iStartLevel )))
      return -1;
    // ��������� � �������� ������������

    /// .... todo ....
    return 0;
  }

  //int // ����� �����, (-1 ���� �� ��������� ������� ���������)
  //  findNearest(
  //  const PointType& pt, // ����� ��� ������� ���� ����� � ��������� �������
  //  int iStartSphere = 0,// � ����� ����� �������� �����, 0 - ������ ������ 

  //  /// .... todo ....
  //  return 0;
  //}

  void printNode( int node, int level )
  {
    spheres[node].print(level);
    for ( int kid = spheres[node].last_kid; kid > 0; kid = spheres[kid].prev_brother )
    {
          printNode( kid, level+1 );
    }
  }

  void reportStatistics( int node =0,  int detailsLevel=2 ) 
    // 0-none, 1-overall statistics, 2-by levels, 3-print tree hierarchy, 4-print tree array with links
  {
    if (detailsLevel < 1)
      return; // set breakpoint here for details

    int maxLevel=-1; // ��� �� �������
    for (int i=0; i< int( spheres.size() ); i++)
      maxLevel = std::max( maxLevel, spheres[i].level );

    std::cout 
      << "********** CoverTree: " << std::endl 
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
        << "level=" << i 
        << "\tradius=" << getRadius(i) 
        << "\tcount of spheres=" << spheresByLevel[i] 
        << "\tpoints="  << pointsByLevel[i]  << "\taverage=" << double( pointsByLevel[i])/ spheresByLevel[i]
        << "\tkids="    << kidsByLevel[i]    << "\taverage=" << double( kidsByLevel[i])/ spheresByLevel[i]
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

};




#endif // __COVER_TREE_H

