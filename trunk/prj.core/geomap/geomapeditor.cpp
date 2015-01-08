//geomapeditor.cpp
#include "ocvutils/precomp.h"
#include "geomap/geomapeditor.h"

void mouseCallBack4GeoMapEditor(int event, int x, int y, int flags, void *userdata)
{
  GeoMapEditor* mw = (GeoMapEditor*)userdata;
  mw->processMouseEvent(event, x, y, flags);
}

GeoMapEditor::GeoMapEditor( const char* _root_folder ) /// = NULL )
  : title("map_view")
{ 
  namedWindow( title, WINDOW_NORMAL ); //WINDOW_AUTOSIZE); // -- в режиме AUTOSIZE координаты x y мышки надо пересчитывать
  setMouseCallback( title, mouseCallBack4GeoMapEditor, this);

  if (_root_folder!=NULL)
  {
    gm.import(_root_folder); // импорт имеет преференцию перед тем что было.
    gm.read(_root_folder); // в момент чтения если лист карты уже _импортирован_ (по имени) то старый не вкл
  }
};


void GeoMapEditor::update_location( Point2d en )
{
  location = en;
  cur_sheet = gm.find_best_sheet(en);
  draw();
}

void GeoMapEditor::draw()
{
  if (cur_sheet >= 0 && cur_sheet < int(gm.sheets.size()) )
  {
    Mat draw = gm.sheets[cur_sheet].raster.clone();
    circle( draw, gm.sheets[cur_sheet].en2xy( location ), 5, Scalar( 0, 0, 255 ), 2 );
    for (int i=0; i< int(gm.objects.size()); i++)
    {
      AGMObject* pagmo = gm.objects[i];
      for (int j=0; j< int(pagmo->pts.size()); j++)
      {
        Point pt = gm.sheets[cur_sheet].en2xy( pagmo->pts[j] );
        circle( draw, pt, 5, Scalar( 255, 0, 255 ), 2 );
      }
    }
    imshow( title, draw );
    //waitKey(1);
  }
}


int GeoMapEditor::finishMouseEvent() // евент обработан, надо отрисовать и закончить его обработку
{
  draw();
  return 0;
      //update_image_to_draw();
      //draw_image_dirty = true;
      //update_window(true);
      //return 0;
}


#define ADD_OBJECT_RECT      0x0001 // пытаемся добавить прямоугольник
//#define ADD_OBJECT_SEGM      0x0002 // пытаемся добавить отрезок
//#define ADD_OBJECT_LINE      0x0004 // пытаемся добавить линию
//#define ADD_OBJECT_QUAD      0x0008  // пытаемся добавить квадрангл
//..

#define ADD_OBJECT_MOUSE_LEFT   0x0100 // инициируем левой кнопкой мыши
#define ADD_OBJECT_MOUSE_RIGHT  0x0200 // инициируем правой кнопкой мыши
#define ADD_OBJECT_MOUSE_SHIFT  0x0400 // инициируем с зажатым SHIFT
#define ADD_OBJECT_MOUSE_CTRL   0x0800 // инициируем с зажатым CONTROL

#define ADD_OBJECT_TRACKED      0x1000 // автоматическое добавление в режиме трекинга


int GeoMapEditor::processMouseEvent(int event, int x, int y, int flags)
{

  //switch (mouseScenario)
  //{

  //}

  //cout << "event=" << event << " coords=" << x << " " << y << " flags=" << flags << endl;

  string objtype = objType(); // тип объекта, с которым работаем
  bool object_poligonal = (objtype == "AFO_Segm" || objtype == "AFO_Triangle" || objtype == "AFO_Quad" || objtype == "AFO_Polygon");
  ///bool object_segment_based = (objtype == "AFO_Segm"); // либо производные двух точечные отрезкообразные
  cv::Point curPoint(x,y);
  int max_poly_points=1000;
  if (objtype == "AFO_Segm")
    max_poly_points=2;
  if (objtype == "AFO_Triangle")
    max_poly_points=3;
  if (objtype == "AFO_Quad")
    max_poly_points=4;

  cout << "GeoMap:objtype" << objtype << endl;

  switch (event)
  {
    case CV_EVENT_LBUTTONDOWN   : // =1,0,
    case CV_EVENT_RBUTTONDOWN   : // =1,0,
      if (object_poligonal)
      {
        if ( rubbering_pts.size() == 0 ) // первая точка
        {
          rubbering_pts.push_back(curPoint);
          rubbering_pts.push_back(curPoint); // создали вырожденный первый сегмент, вторую точку будем корректировать на mousemove 
          rubbering_mode = RUBBERING_SEGMENT;
        }
        else if (rubbering_pts.size()>=2 && // если последний сегмент не вырожденный -- начнем новый
          l2norm( rubbering_pts[rubbering_pts.size()-2], rubbering_pts[rubbering_pts.size()-1] ) >= 25  )
        {
          rubbering_pts.push_back(curPoint);
          rubbering_mode = RUBBERING_SEGMENT;
        }             
      }
      else
      {
        rubbering_pts.clear();
        rubbering_pts.push_back(curPoint);
        rubbering_pts.push_back(curPoint); // создали вырожденный ректангл 
        rubbering_mode = RUBBERING_RECTANGLE; // пока так, грубо.
      }
      rubber_by_left_button = (event == CV_EVENT_LBUTTONDOWN);
      return finishMouseEvent();

    case CV_EVENT_MOUSEMOVE     : // =0,
      if (rubbering_mode == RUBBERING_RECTANGLE)
      {
        assert(rubbering_pts.size() == 2);
        rubbering_pts[1] = curPoint; // обновили точку
        return finishMouseEvent();
      }
      if (rubbering_mode == RUBBERING_SEGMENT)
      {
        assert(rubbering_pts.size() >=2);
        ////if (rubbering_pts.size() == 2) 
          rubbering_pts.back() = curPoint; // обновили точку
        return finishMouseEvent();
      }
      if (rubbering_mode == RUBBERING_POLYGON) // уже вытянута часть многоугольника, и мы свободно водим мышкой планируя начать вытягивать следующее ребро многоугольника
      {
        assert(rubbering_pts.size() >=2);
        // ничего не делаем, пока выбор пользователя не понятен и отрисовка не меняется
        //return 0;
        rubbering_pts.back() = curPoint;
        return finishMouseEvent();
      }

    //case CV_EVENT_RBUTTONDOWN   : // =2,
    //case CV_EVENT_MBUTTONDOWN   : // =3,
    case CV_EVENT_LBUTTONUP     : // =4,
    case CV_EVENT_RBUTTONUP     : // =4,
      if (rubbering_mode == RUBBERING_RECTANGLE) 
      {
        assert(rubbering_pts.size() == 2);
        rubbering_pts[1] = curPoint; // скорректировали
        cv::Rect rect(rubbering_pts[0], rubbering_pts[1]);
        //cout << "rect.x =" << rect.x << " rect.y =" << rect.y;
        //cout << "rect.width =" << rect.width << " rect.height =" << rect.height << endl;
        int flags = ADD_OBJECT_RECT | (rubber_by_left_button ? ADD_OBJECT_MOUSE_LEFT : ADD_OBJECT_MOUSE_RIGHT);
        rubbering_mode = RUBBERING_NOTHING;
        rubbering_pts.clear();
        return addMouseObject(rect, flags);
      }

      if (rubbering_mode == RUBBERING_SEGMENT)
      {
        assert(rubbering_pts.size() >= 2);
        rubbering_pts.back() = curPoint;
        if ( l2norm( rubbering_pts[0], curPoint) < 100 // замкнули или просто клик был. конец банкета
          //////||
          //////  (object_segment_based && rubbering_pts.size() == 2 )
          || rubbering_pts.size() == max_poly_points+1
            ) 
        {
          if (rubbering_pts.size() > 2)
            rubbering_pts.back() = rubbering_pts[0]; // скорректировали последнюю точку прилепив к начальной
          int flags = ADD_OBJECT_RECT | (rubber_by_left_button ? ADD_OBJECT_MOUSE_LEFT : ADD_OBJECT_MOUSE_RIGHT);
          if (l2norm( rubbering_pts[rubbering_pts.size()-2], rubbering_pts[rubbering_pts.size()-1] ) < 25)
          { // удаляем последнее ребро если оно вырожденное
            rubbering_pts[rubbering_pts.size()-2] = rubbering_pts[0];
            rubbering_pts.pop_back();
          }
          rubbering_mode = RUBBERING_NOTHING;
          int res = addMouseObject(rubbering_pts, flags);
          rubbering_pts.clear();
          return res;
        }
        // не замкнулись, продолжаем, надо перерисовать
        rubbering_pts.back() = curPoint;
        rubbering_mode = RUBBERING_POLYGON;

        return finishMouseEvent();
      }
      break;
    //case CV_EVENT_RBUTTONUP     : // =5,
    //case CV_EVENT_MBUTTONUP     : // =6,
    //case CV_EVENT_LBUTTONDBLCLK : // =7,
    //case CV_EVENT_RBUTTONDBLCLK : // =8,
    //case CV_EVENT_MBUTTONDBLCLK : // =9
  }
  return finishMouseEvent();
}

bool GeoMapEditor::addMouseObject( // пытаемся добавить новый объект вытянув или кликнув мышкой
    cv::Rect& rect, // note: in-out -- подкручиваем ректангл по законам первого рождения для данного объекта
    int flags )
{
  Point xy = center( rect );
  GeoSheet& sh = gm.sheets[ cur_sheet ];
  Point2d en = sh.xy2en( xy );
  AGM_Point* pp = new AGM_Point( en );
  gm.objects.push_back(cv::Ptr<AGM_Point>(pp));
  return true;
};


bool GeoMapEditor::addMouseObject( // пытаемся добавить новый объект вытянув или кликнув мышкой
  std::vector< cv::Point >& pts, // note: in-out -- подкручиваем точки по законам первого рождения для данного объекта
  int flags )
{
  return __false("GeoMapEditor::addMouseObject(std::vector< cv::Point >& pts) not implemented");
};