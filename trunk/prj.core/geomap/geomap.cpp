#include "ocvutils/precomp.h"
#include "geomap/geomapeditor.h"


bool GeoSheet::create( const char* sheet_file_name )
{
  raster = imread( sheet_file_name ); // todo try { }
  if (raster.empty()) 
    return false;  
  return true;
}	

bool GeoMap::import( const char * _root_folder )
{
  root_folder = _root_folder;
  string sheets_list_file = root_folder + "/sheets.map";
  ifstream ifs( sheets_list_file.c_str() );
  if (!ifs.is_open())
    return false;
  int nsheets = 0;
  ifs >> nsheets;
  if (nsheets <=0 || nsheets > 1024)
    return false;
  sheets.resize(nsheets);
  for ( int i=0; i<nsheets; i++ )
  {
    GeoSheet& sh = sheets[i]; 
    ifs >> sh.sheet_name;
    string sheet_path = root_folder + "/" + sh.sheet_name;
    if ( !sh.create( sheet_path.c_str() ))
      return false;
    ifs >> sh.a.xy.x >> sh.a.xy.y >> sh.a.en.y >> sh.a.en.x; 
    ifs >> sh.b.xy.x >> sh.b.xy.y >> sh.b.en.y >> sh.b.en.x; 
  }
  return true;
}

bool GeoMap::read( const char * _root_folder )
{
  root_folder = _root_folder;
  string filename = root_folder + "/geomap.yaml";
  cv::FileStorage fs( filename, cv::FileStorage::READ );
  if (!fs.isOpened())
    return __false( format("Can't open storage '%s' to read", filename.c_str() ) );

  if (!read(fs))
    return false;
  return true;
}

bool GeoMap::write()
{
  string filename = root_folder + "/geomap.yaml";
  cv::FileStorage fs( filename, cv::FileStorage::WRITE );
  if (!fs.isOpened())
    return __false( format("Can't open storage '%s' to write", filename.c_str() ) );

  if (!write(fs))
    return false;
  return true;
}

int GeoMap::find_best_sheet( Point2d en )
{
  double best_dd = 40000000.; // 40 000 000 метров, будем улучшать
  int _best_sheet =-1;
  for (int i=0; i< int(sheets.size()); i++)
  {
    GeoSheet& sh = sheets[i];
    assert(sh.raster.cols>0);
    assert(sh.raster.rows>0);

    Point xy = sh.en2xy( en );
    double dx = abs( double( sh.raster.cols/2 - xy.x ) / sh.raster.cols );
    double dy = abs( double( sh.raster.rows/2 - xy.y ) / sh.raster.rows );
    if (dx > 0.5 || dy > 0.5) // точка за пределами листа карты
      continue;
#if 0 
    // старый вариант подматывает карту, в которой точка ближе к центру битмапа __в пикселях__
    double dd = max( dx, dy );
#else
    // новый вариант подматывает карту, в которой точка ближе к центру битмапа __в метрах__
    Point2d delta_meters = sh.sheet_center_mercator( en );
    double dd = sqrt( delta_meters.ddot( delta_meters ) );
#endif

    if (dd < best_dd)
    {
      best_dd = dd;
      cout  << "Distance to map sheet " << i << " center:" << best_dd << endl;
      _best_sheet = i;
    }
  }
  return _best_sheet;
}

bool GeoSheet::write( cv::FileStorage& fs )
{
  fs << "{:";
  fs << "sheet_name" << sheet_name;
  fs << "reper_a_xy" << a.xy;
  fs << "reper_a_en" << a.en;
  fs << "reper_b_xy" << b.xy;
  fs << "reper_b_en" << b.en;
  fs << "}";
  return true; // todo __try
}

bool GeoSheet::read( cv::FileNode& fn )
{
  if (fn.empty())
    return false;

  fn[ "sheet_name" ] >> sheet_name;
  fn[ "reper_a_xy" ] >> a.xy;
  fn[ "reper_a_en" ] >> a.en;
  fn[ "reper_b_xy" ] >> b.xy;
  fn[ "reper_b_en" ] >> b.en;

  return true; // todo __try
}

bool GeoMap::read( cv::FileStorage& fs )
{ 
  cv::FileNode& node = fs.root(); 
  if (node.empty())
    return __false();

  {
    cv::FileNode sheets_node = node["GeoMapSheets"];
    for (cv::FileNodeIterator it = sheets_node.begin(); it != sheets_node.end(); ++it)
    {
      GeoSheet sh; 
      if (!sh.read(*it))
        return __false("Cannot read GeoMap sheet");

      string sheet_path = root_folder + "/" + sh.sheet_name;
      if ( !sh.create( sheet_path.c_str() ))
        return false;
      // проверим нет ли такого листа в карте. если уже есть -- проигнорируем.
      bool found=false;
      for (int i=0; i<int(sheets.size()); i++)
      {
        if (sheets[i].sheet_name == sh.sheet_name)
        {
          found=true;
          break;
        }
      }
      if (!found)
        sheets.push_back( sh );
    }
  }
  {
    cv::FileNode obects_node = node["GeoMapObjects"];
    for (cv::FileNodeIterator it = obects_node.begin(); it != obects_node.end(); ++it)
    {
      AGMObject* obj = ReadAGMObject(*it);
      if (obj)
        objects.push_back(cv::Ptr<AGMObject>(obj));
      //else
      //  assert(0); // ??
    }
  }
  return true;
}

bool GeoMap::write( cv::FileStorage& fs )
{
  fs << "GeoMapSheets" << "[";
  for (int i=0; i<int(sheets.size()); i++)
    if (!sheets[i].write( fs ))
      return __false("Can't write GeoMap sheet");
  fs << "]";

  fs << "GeoMapObjects" << "[";
  for (int i=0; i<int(objects.size()); i++)
    WriteAGMObject( fs, objects[i] ); // => корневой write() отпишет базовые данные и вызовет writeSelf() для производного
  fs << "]";

  return true;
}



AGMObject* CreateAGMObject( GMObject& gmo ) //фабрика
{
  if (gmo.type == "AGM_Point" )
    return new AGM_Point( gmo );
  if (gmo.type == "AGM_Segm" )
    return new AGM_Segm( gmo );
  return NULL;
}

AGMObject* ReadAGMObject(cv::FileNode &node) // фабрика-читальня c использованием виртуального readSelf()
{ 
  GMObject gmo;
  cv::Rect rect;
  int flags=0; 
  AGMObject* agmo = NULL;
  try {
    node["type"]    >> gmo.type;
    node["pts"]    >> gmo.pts;
    node["flags"]   >> gmo.flags;
    node["tags"]   >> gmo.tags;
    agmo = CreateAGMObject( gmo );
  } 
  catch (...) 
  {
    cout << "readAGMObject() failed" << endl;
    return NULL; // unknown object type or no type tag at all
  }

  if (agmo)
    agmo->readSelf(node); 

  return agmo; 
}

bool WriteAGMObject(cv::FileStorage& fs, AGMObject* agmo ) // запись, инициирует виртуальный writeSelf()
{
  fs << "{:";
  fs << "type"  << agmo->type;
  fs << "pts"   << agmo->pts;
  if (agmo->flags != 0)
    fs << "flags"   << agmo->flags;
  if (!agmo->tags.empty())
    fs << "tags"   << agmo->tags;

  agmo->writeSelf(fs); // virtual -- extended by successors

  fs << "}";

  return true; // todo __try
}


