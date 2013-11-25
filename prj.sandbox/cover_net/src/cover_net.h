// cover_net.h

#ifndef __COVER_NET_H
#define __COVER_NET_H

#include <vector>
//#include <algorithm>

#define COVER_NET_VERBOSE
//#define COVER_NET_VERBOSE_DETAILED

template < class PointType >
struct CoverSphere
{
  int parent;    // родитель // fat -- не обязательно для основных операций
  int prev_brother;   // индекс предыдущего брата; 0 -- нет
  int last_kid;  // указатель на последнего рожденного ребенка; 0 -- нет

  //... ?
  double distance_to_parent; // расстояние до родительской вершины  // fat?
  //std::vector< int > ancles; // вершины уровнем выше, которые покрывают данную // fat fat slow
  int ancle; // индекс начала списка дядьев --> ancles[ancle](дядья в списке идут в порядке возрастания расстояния?)
  //.....

  PointType center;
  int points;   // количество точек, ассоциированных с данной вершиной // fat
  int level;    // уровень вершины в дереве // fat

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
    points(0),
    distance_to_parent(distance_to_parent)
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
class CoverNet
{
  std::vector< CoverSphere< PointType > > spheres;
  std::vector< std::pair< int, int > > ancles; // <ancle sphere, next> списки вершин уровнем выше, которые покрывают данную 

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


  CoverNet( Metrics* ruler, double rootRadius, double minRadius, 
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
#ifdef COVER_NET_VERBOSE_DETAILED
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

//#define DONT_FORCE_DIRECT_SUCCESSORS
  int makeSphere( // создает сферу, ее прямых наследников и возвращает ее номер
    int iSphereLevel, // уровень создаваемой сферы
    const PointType& pt, // центр сферы
    int parent, // родительская сфера (гарантируется, что центр лежит внутри родительской и не покрыт братьями
    double distance_to_parent // расстояние до центра родительской сферы
    ) 
  {
    do {
      spheres.push_back( CoverSphere<PointType>( iSphereLevel, pt, parent, distance_to_parent ) );
      iLastSphere = spheres.size()-1;
      iLastSphereLevel = iSphereLevel;
      if (parent >=0) // not root
      {
#ifdef DONT_FORCE_DIRECT_SUCCESSORS 
        int bro = spheres[parent].last_kid; // поддержка списка детей упорядоченных по расстоянию от родителя???
        spheres[parent].last_kid = iLastSphere;
        spheres[iLastSphere].prev_brother = bro;
#else
		int bro = spheres[parent].last_kid;//последний ребенок
		if (bro == 0) // bro нету
		{
			spheres[parent].last_kid = iLastSphere;//тогда iLastSphere -- первый ребенок
			spheres[iLastSphere].prev_brother = bro;// = 0;
		}
		else
		{
			int prev_bro = spheres[bro].prev_brother;//предпоследний ребенок
			spheres[bro].prev_brother = iLastSphere;// предпоследний теперь новый
			spheres[iLastSphere].prev_brother = prev_bro;//перед новым стоит prev_bro
		}
#endif
      }
#ifdef DONT_FORCE_DIRECT_SUCCESSORS
      break;
#endif
      iSphereLevel++;
      parent = iLastSphere;
      distance_to_parent = 0;
    } while ( getRadius(iSphereLevel) > getMinRadius(iSphereLevel) );

    notifyParents( pt, parent, iSphereLevel, SPHERE_CREATED );
    return iLastSphere;
  }

  void makeRoot( const PointType& pt )  {   makeSphere( 0, pt, -1, 0 );  }

  void insertPoint( 
    const PointType& pt, // точка внутри сферы iSphere на уровне iSphereLevel
    int iSphere, 
    int iSphereLevel, 
    double dist // расстояние до центра сферы iSphere (уже измерено)
    )
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
        insertPoint( pt, kid, iSphereLevel+1, dist ); // провалились
        return; // конец банкета
      }
      kid = spheres[kid].prev_brother;
    }
    // нет детей, место свободно, создаем сферу и, возможно, ее прямых наследников
    makeSphere( iSphereLevel+1,  pt, iSphere, dist );

  }

  void attachPoint( const PointType& pt, int iSphere, int iSphereLevel, double dist )
    // лепим непосредственно к данной сфере, не пытаясь свалиться вниз
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

#if 1 // ----------------------------------- not implemented yet 

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

  public:
  const PointType& 
  findNearestPoint( // ближайший к указанной точке центр сферы из дерева
    const PointType& pt, // точка для которой ищем сферу с ближайшим центром
    double &best_distance// [in/out] рекорд расстояния -- оно же и отсечение (не искать дальше чем указано)
  , int iStartSphere = 0// с какой сферы начинать поиск, 0 - корень дерева 
  )
  {
    
    int iNearestSphere = findNearestSphere( pt, best_distance, iStartSphere);
	  if (iNearestSphere == -1)
	  {
		  cerr << "error: distance to root is more then maximal radius" << endl;
	  }

	  return spheres[iNearestSphere].center;
  }
 
  int // номер сферы, (-1 если за пределами радиуса стартовой)
  findNearestSphere(
    const PointType& pt, // точка для которой ищем сферу с ближайшим центром
    double& best_distance // [in/out] рекорд расстояния -- оно же и отсечение (не искать дальше чем указано)
  ,  int iStartSphere = 0// с какой сферы начинать поиск, 0 - корень дерева 
  )
  {
    int isp=iStartSphere; // текущая сфера
	  int lev = spheres[isp].level; // текущий уровень
    double  rad = getRadius(lev);// радиус текущей сферы (на данном уровне)
    double dist = computeDistance( isp, pt );
	
	  if (isp == 0 && dist > rad)// если за пределами стартовой
		  return -1;

	  int ans = -1;
	  double min_dist = max(0.0, dist - rad);// расстояние от pt до сферы  
	  if (min_dist > best_distance)// если минимальное расстояние больше оптимального
		  return -1;
	  if (dist < best_distance)// если можно улучшить ответ
	  {
		  ans = isp;
		  best_distance = dist;
	  }

	  for (int kid = spheres[isp].last_kid; kid > 0; kid = spheres[kid].prev_brother)// идем по всем детям
	  {
		  int kid_ans = findNearestSphere(pt, best_distance, kid);
		  if (kid_ans != -1)// если результат улучшился 
			  ans = kid_ans;
	  }

	  return ans;
  }

  public:
  int // номер сферы
  dropToNearestKid( // рекурсивное "проваливание" в ближайшую детскую сферу. 
                    // на входе -- точка внутри или вне родительской сферы
                    // на выходе -- номер найденной сферы нижнего уровня и расстояние до нее
    int isphere, // проваливаемся в данную сферу
    const PointType& pt, // точка для которой ищем сферу с ближайшим центром
    double* best_distance // [in/out] рекорд расстояния -- оно же и отсечение (не искать дальше чем указано)
    )
  {
    // пока есть ребенок, в которого попадаем (из нескольких -- ближайший) -- проваливаемся в него
    return 0; // todo
  }
#endif // ---------------------- not implemented yet

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
      << "********** CoverNet: " << std::endl 
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

};




#endif // __COVER_NET_H

