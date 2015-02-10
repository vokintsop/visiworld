#ifndef __GSTORAGE_H
#define __GSTORAGE_H

#include <vector>
#include <cassert>
#include <string>
#include <set>

#include <opencv2/imgproc/imgproc.hpp>

const double EPS_DOUBLE = 0.0001;

const int GST_NO_ERROR = 1;
const int GST_ERROR = 0;

struct ValueIndex
{
  double val;
  int index;
  ValueIndex(double coord_in, int ind_in) : val(coord_in), index(ind_in) {}
  ValueIndex(const ValueIndex& in) { val = in.val; index = in.index; }
  bool operator<(const ValueIndex& robj) const
  {
    return (val < robj.val);
  }
};

typedef std::set<ValueIndex> TSetValueIndex;

class GStorage;
struct GMObject;

typedef cv::Point2d ENPoint2d; // точка в координатах EN с двойной точностью (x=east, y=nord) 
typedef cv::Point3d ENUPoint3d; // точка в координатах ENU с двойной точностью (x=east, y=nord, z=up)

// надо сделать расстояние между объектами функцией GMObject, чтобы при переходе на 3D не менять код
// или template для типа координат, но это хуже
class GStorageIndex
{
public:

  GStorageIndex() : m_pStorage(0) {};
  virtual ~GStorageIndex() {};

  // Связь класса с хранилищем объектов
  int attach(GStorage* pStorage);
  int detach();

  int insert(GMObject* element);

  int nearestKSearch (const GMObject &point, int k, 
    std::vector<int> &k_indices, std::vector<double> &k_sqr_distances) const;

  int radiusSearch (const GMObject &point, int k,  double radius,
    std::vector<int> &k_indices, std::vector<double> &k_sqr_distances) const;

private:
  int addObj(GMObject* obj, int index);

private:

  std::vector<ENPoint2d> m_vecGObjs;
  
  GStorage* m_pStorage;

  TSetValueIndex m_xInd;
  TSetValueIndex m_yInd;
};

class GStorage
{
public:

  GStorage() {};
  virtual ~GStorage() {};

  int load(const char* file);
  bool read( cv::FileStorage& fs );

  int save(const char* file);

  friend int GStorageIndex::insert(GMObject* element); // синхронизация с индексом

  int size() { return m_vecGObjs.size(); }
  GMObject* operator[](int ind);

private:

  int insert(GMObject* element);  // вызов либо при загрузке либо индексом

  std::vector<GMObject*> m_vecGObjs;

  std::string root_folder;
};

#endif // __GSTORAGE_H


