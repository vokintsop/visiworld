#include "gstorage.h"
#include "geomap/geomap.h"

//=========================================================================================

int GStorage
::load(const char* file)
{
  root_folder = file;//_root_folder;
  std::string filename = root_folder + "/geomap.yaml";
  cv::FileStorage fs( filename, cv::FileStorage::READ );
  if (!fs.isOpened())
    return GST_ERROR;//__false( format("Can't open storage '%s' to read", filename.c_str() ) );

  if (!read(fs))
    return GST_ERROR;
  return GST_NO_ERROR;
}

bool GStorage::read( cv::FileStorage& fs )
{ 
  const cv::FileNode& node = fs.root();
  if (node.empty())
    return GST_ERROR;//__false();

//   {
//     cv::FileNode sheets_node = node["GeoMapSheets"];
//     for (cv::FileNodeIterator it = sheets_node.begin(); it != sheets_node.end(); ++it)
//     {
//       GeoSheet sh; 
//       if (!sh.read(*it))
//         return GST_ERROR;//__false("Cannot read GeoMap sheet");
// 
//       string sheet_path = root_folder + "/" + sh.sheet_name;
//       if ( !sh.create( sheet_path.c_str() ))
//         return false;
//       // проверим нет ли такого листа в карте. если уже есть -- проигнорируем.
//       bool found=false;
//       for (int i=0; i<int(sheets.size()); i++)
//       {
//         if (sheets[i].sheet_name == sh.sheet_name)
//         {
//           found=true;
//           break;
//         }
//       }
//       if (!found)
//         sheets.push_back( sh );
//     }
//   }

  {
    cv::FileNode obects_node = node["GeoMapObjects"];
    for (cv::FileNodeIterator it = obects_node.begin(); it != obects_node.end(); ++it)
    {
      AGMObject* obj = ReadAGMObject(*it);
      if (obj)
        //objects.push_back(cv::Ptr<AGMObject>(obj));
        m_vecGObjs.push_back(obj);
    }
  }
  return true;
}

int GStorage
::save(const char* file)
{
  return GST_NO_ERROR;
}

int GStorage
::insert(GMObject* element)
{
  m_vecGObjs.push_back(element);
  return m_vecGObjs.size();
}

GMObject* GStorage::operator[](int ind)
{
  if (m_vecGObjs.size() == 0)
    return 0;
  return m_vecGObjs[ind];
}
//=========================================================================================

static bool centerOf(const GMObject* obj, ENPoint2d& enp)
{
  int sz = obj->pts.size();
  if (sz == 0)
    return GST_ERROR;
  if (sz == 1)
  {
    enp = obj->pts[0];
    return GST_NO_ERROR;
  }

  enp.x = 0;
  enp.y = 0;
  for (int i = 0; i < sz; i++)
  {
    enp.x += obj->pts[i].x;
    enp.y += obj->pts[i].y;
  }
  enp.x /= sz;
  enp.y /= sz;

  return GST_NO_ERROR;
}

int GStorageIndex::addObj(GMObject* obj, int index)
{
   ENPoint2d enp;
   
   if (false == centerOf( obj, enp))
     return GST_ERROR;

   ValueIndex xc(enp.x, index);
   ValueIndex yc(enp.y, index);
   m_xInd.insert(xc);
   m_yInd.insert(yc);
   m_vecGObjs.push_back(enp);

   return GST_NO_ERROR;
}

// Связь класса с хранилищем объектов
int GStorageIndex::attach(GStorage* pStorage)
{
  int sz = pStorage->size();
  if (sz == 0)
    return GST_ERROR;

  int failedObjects = 0;
  for (int i = 0; i < sz; i++)
  {
    if  (GST_NO_ERROR != addObj( (*pStorage)[i], i) )
      failedObjects++;
  }
  return (sz - failedObjects);
}

int GStorageIndex::detach()
{
  m_vecGObjs.clear();
  m_xInd.clear();
  m_yInd.clear();
  m_pStorage = 0;
  return GST_NO_ERROR;
}

int GStorageIndex
::insert(GMObject* obj)
{
  if (m_pStorage == 0)
    return GST_ERROR;

  int index = m_pStorage->insert(obj) - 1;

  return addObj(obj, index);
}

static double caldSqrDist(const ENPoint2d& pt1, const ENPoint2d& pt2)
{
  return sqrt((pt1.x - pt2.y)*(pt1.x - pt2.y) 
    + (pt1.y - pt2.y)*(pt1.y - pt2.y));
}

int GStorageIndex
::nearestKSearch (const GMObject &point, int k, 
    std::vector<int> &k_indices, std::vector<double> &k_sqr_distances) const
{
  int sz = m_vecGObjs.size();
  TSetValueIndex dists;
  
  ENPoint2d enpCenter;
  if (false == centerOf(&point, enpCenter))
    return GST_ERROR;;

  for (int i = 0; i < sz; i++)
  {
    dists.insert(ValueIndex(
                       caldSqrDist(enpCenter, m_vecGObjs[i]),
                       i
                       )
                );
  }

  int cntElem = (k < dists.size()) ? k : dists.size();
  k_indices.resize(cntElem);
  k_sqr_distances.resize(cntElem);

  TSetValueIndex::iterator it = dists.begin();
  for (int i = 0; i < cntElem; i++)
  {
    ValueIndex c = (*it++);
    k_indices[i] = c.index;
    k_sqr_distances[i] = c.val;
  }
  return cntElem;
}

int GStorageIndex
::radiusSearch (const GMObject &point, int k, double radius,
    std::vector<int> &k_indices, std::vector<double> &k_sqr_distances) const
{
  ENPoint2d enpCenter;
  if (false == centerOf(&point, enpCenter))
    return GST_ERROR;;

  TSetValueIndex::const_iterator itXLower = m_xInd.lower_bound(ValueIndex(enpCenter.x - radius, 0));
  TSetValueIndex::const_iterator itXUpper = m_xInd.upper_bound(ValueIndex(enpCenter.x +  radius, 0));

  TSetValueIndex xInds(itXLower, itXUpper);

  TSetValueIndex::const_iterator itYLower = m_yInd.lower_bound(ValueIndex(enpCenter.y - radius, 0));
  TSetValueIndex::const_iterator itYUpper = m_yInd.upper_bound(ValueIndex(enpCenter.y + radius, 0));

  TSetValueIndex yInds(itYLower, itYUpper);

  for (TSetValueIndex::iterator itX = xInds.begin(); itX != xInds.end(); itX++)
  {
    for (TSetValueIndex::iterator itY = yInds.begin(); itY != yInds.end(); itY++)
    {
      if ((*itX).index == (*itY).index)
      {
        double dist = sqrt((enpCenter.x - (*itX).val)*(enpCenter.x - (*itX).val) 
                         + (enpCenter.y - (*itY).val)*(enpCenter.y - (*itY).val));
        if (radius + EPS_DOUBLE > dist)
        {
          k_indices.push_back((*itX).index);
          k_sqr_distances.push_back(dist);
        }
      }
    }
  }

  return k_indices.size();
}

