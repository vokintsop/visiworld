// cover_tree.h

#ifndef __COVER_TREE_H
#define __COVER_TREE_H

#include <vector>

#define COVER_TREE_VERBOSE
//#define COVER_TREE_VERBOSE_DETAILED

template < class PointType >
struct CoverSphere
{
  int parent;    // родитель // fat -- не обязательно для основных операций
  int prev_brother;   // индекс предыдущего брата; 0 -- нет
  int last_kid;  // указатель на последнего рожденного ребенка; 0 -- нет

  PointType center;
  int points;   // количество точек, ассоциированных с данной вершиной // fat
  int level;    // уровень вершины в дереве // fat

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

struct CoverRecord // регистрация рекордного расстояния
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
  double rootRadius;  // при создании рута изначально указывается ожидаемое расстояние между наиболее удаленными точками
                      // если встречается точка, удаленная от рута на расстояние больше rootRadius дерево надстраивается вверх
  double minRadius;   // если расстояние до центра сферы меньше minRadius вложенные сферы не порождаются
  double maxRadius;   // если расстояние до центра сферы больше maxRadius точка игнорируется

  int iLastSphere;  // номер сферы, к которой была последний раз присоединена точка
  int iLastSphereLevel; // 0 -- рут
  //double lastComputedDistance; // значение последнего вычисленного расстояния

  ////int iRootSphere;  // индекс поддерева, _объявленного_ корнем "селектированного поддерева" 
  ////int iRootLevel;   // уровень корня "селектированного поддерева"
  //// задумка селектированного поддерева состоит в ограничении операции только рамками данного поддерева,
  //// для сокращения числа параметров рекурсивно вызываемых функций 

  int attemptsToInsert; // счетчик числа попыток вставки
  int rejectedInserts; // счетчик числа попыток вставки слишком далеких точек

  double squeezeRatio; // коэффициент уменьшения вложенных окружностей

  std::vector< double > levelsRadii;
public:
  double getRadius( int level ) { return levelsRadii[level]; }
  double getMinRadius( int level ) { return minRadius; }
  //std::vector< double > levelsMinRadii;


  CoverTree( Metrics* ruler, double rootRadius, double minRadius, 
    double squeezeRatio = 0.5, // коэффициент уменьшения вложенных окружностей
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

    attemptsToInsert=0; // счетчик числа попыток вставки
    rejectedInserts=0; // счетчик числа попыток вставки слишком далеких точек
  };

  bool insert( const PointType& pt ) // false -- лежит за пределами центра нашей вселенной (радиуса рута)
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
      return false; // игнорируем слишком далекую точку
    }

#ifndef NO_SEQUENTAL_PROXIMITY_ASSUMPTION  // если регистрируем пиксели подряд, то соседи вероятно близки
    if (iLastSphere != -1)
    {
      double dist = computeDistance( iLastSphere, pt );
      double rad = getRadius(iLastSphereLevel);
      if ( dist < rad )
      {
        int start=iLastSphere;
        insertPoint( pt, iLastSphere, iLastSphereLevel, dist ); // изменяет iLastSphere & iLastSphereLevel
        for (int isp = spheres[start].parent; isp >=0; isp = spheres[isp].parent)
          spheres[isp].points++;
        return true;
      }
    }
#endif

    insertPoint( pt, 0, 0, dist_root ); // добавляем спускаясь с рута

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

  int makeSphere( const PointType& pt, int parent, int iSphereLevel ) // создает сферу и возвращает ее номер
  {
    spheres.push_back( CoverSphere<PointType>( parent, pt, iSphereLevel ) );
    iLastSphere = spheres.size()-1;
    iLastSphereLevel = iSphereLevel;
    notifyParents( pt, parent, iSphereLevel, SPHERE_CREATED );
    return iLastSphere;
  }

  void makeRoot( const PointType& pt )  {   makeSphere( pt, -1, 0 );  }

  void insertPoint( const PointType& pt, int iSphere, int iSphereLevel, double dist )
  { // мы внутри сферы
    spheres[iSphere].points++;

    double minrad = getMinRadius(iSphereLevel);
    if (dist < minrad)
    { // лепим непосредственно к данной сфере
      attachPoint( pt, iSphere, iSphereLevel, dist ); // лепим непосредственно к данной сфере, не пытаясь свалиться вниз
      return;
    }

    // поищем нет ли ребенка
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

    // создаем сферу
    int isp = makeSphere( pt, iSphere, iSphereLevel+1 );
    int bro = spheres[iSphere].last_kid;
    spheres[iSphere].last_kid = isp;
    spheres[isp].prev_brother = bro;
  }

  void attachPoint( const PointType& pt, int iSphere, int iSphereLevel, double dist )
    // лепим непосредственно к данной сфере, не пытаясь свалиться вниз
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

  // ---- запросы и навигация ----
public:  
  double computeDistance( int iSphere,  const PointType& pt )
  {
    double dist = ruler->computeDistance( spheres[iSphere].center, pt );
    return dist;
  }

  int getSpheresCount() { return int( spheres.size() ); };
  const CoverSphere< PointType >& getSphere( int iSphere ) { return spheres[iSphere]; };
  int countKids( int iSphere ) // подсчитывает количество непосредственных детей у вершины
  { 
    int count=0;
    for (int kid = spheres[iSphere].last_kid; kid > 0; kid = spheres[kid].prev_brother)
      count++;
    return count; 
  };

  int // номер сферы, (-1 если за пределами радиуса стартовой), полученный выполняя        
  branchSubTreeUsingFirstCover(   // прямолинейный спуск по поддереву -- первая фаза branch&bounds
    const PointType& pt, // точка для которой ищем сферу по схеме -- если накрывает -- сразу спускаемся 
                         // т.е. без попытки поиска более близкой на том же уровне 
    int iStartSphere = 0,// с какой сферы начинать поиск, 0 - корень дерева 
    int iStartLevel = 0 // уровень стартовой сферы, 0 - уровень кореня дерева 
//    ,CoverRecord& record // регистрируем рекорд расстояния?
    ) 
  {
    double dist = computeDistance( iStartSphere, pt );
//  record.update( dist, iStartSphere, iStartLevel );
    if (!(dist < getRadius( iStartLevel )))
      return -1;
    // находимся в покрытом пространстве

    /// .... todo ....
    return 0;
  }

  //int // номер сферы, (-1 если за пределами радиуса стартовой)
  //  findNearest(
  //  const PointType& pt, // точка для которой ищем сферу с ближайшим центром
  //  int iStartSphere = 0,// с какой сферы начинать поиск, 0 - корень дерева 

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

    int maxLevel=-1; // рут на нулевом
    for (int i=0; i< int( spheres.size() ); i++)
      maxLevel = std::max( maxLevel, spheres[i].level );

    std::cout 
      << "********** CoverTree: " << std::endl 
      << "spheres=" << spheres.size() 
      << "\tlevels=" << maxLevel+1 
      << "\tattemptsToInsert=" << attemptsToInsert // счетчик числа попыток вставки
      << "\trejectedInserts=" << rejectedInserts // счетчик числа попыток вставки слишком далеких точек
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

