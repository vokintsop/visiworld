//geomapeditor.cpp
#include "ocvutils/precomp.h"
#include "geomap/geomapeditor.h"

#define GEOMAP_EDITOR_TITLE "map_view"


void mouseCallBack4GeoMapEditor(int event, int x, int y, int flags, void *userdata)
{
  GeoMapEditor* mw = (GeoMapEditor*)userdata;
  mw->processMouseEvent(event, x, y, flags);
}

GeoMapEditor::GeoMapEditor( const char* _root_folder ) /// = NULL )
  : iObjType(0), title(GEOMAP_EDITOR_TITLE), rubbering_mode(0)
{ 
  namedWindow( title, WINDOW_NORMAL ); //WINDOW_AUTOSIZE); // -- � ������ AUTOSIZE ���������� x y ����� ���� �������������
  setMouseCallback( title, mouseCallBack4GeoMapEditor, this);

  if (_root_folder!=NULL)
  {
    gm.import(_root_folder); // ������ ����� ����������� ����� ��� ��� ����.
    gm.read(_root_folder); // � ������ ������ ���� ���� ����� ��� _������������_ (�� �����) �� ������ �� ���
  }
};

void GeoMapEditor::update_title()
{
  if (gm.sheets.size() > 0 && cur_sheet>=0)
  {
    string text = format("Map [%d(%s)/%d] type=%s location=Nord:%f East:%f", 
      cur_sheet+1, gm.sheets[cur_sheet].sheet_name.c_str(), gm.sheets.size(), 
      objType().c_str(), // type
      location.y, location.x // location
      );
      //format("Map: %s [#%d of %d, %d msec]; type=%s objects on frame=%d sensitivity=%f tracking=%s", 
      //video_file_name.c_str(),
      //iframe, frames, frame_time, objType().c_str(), numobj, frameProc.sensitivity, tracking_object? "ON" : "OFF" );
    set_window_text( title.c_str(), text.c_str() );
  }
}


void GeoMapEditor::update_location( Point2d en , Vec2d dir )
{
  location = en;
  direction = dir;
  cur_sheet = gm.find_best_sheet(en);
  draw();
}

void GeoMapEditor::draw()
{
  if (cur_sheet >= 0 && cur_sheet < int(gm.sheets.size()) )
  {
    GeoSheet& sh =  gm.sheets[cur_sheet];
    Mat draw = sh.raster.clone();
    Point p1 = sh.en2xy( location );
    Point p2 = Point(p1.x + cvRound(direction[0] * 30), p1.y - cvRound(direction[1] * 30));
    circle( draw, p1, 5, Scalar( 0, 0, 255 ), 2 );
    line( draw, p1, p2, Scalar(0, 0, 255), 2 );

    string text;
    circle( draw, sh.a.xy, 5, Scalar( 0, 255, 255 ), 2 );
    text = "A"; //format( "N:%f.10 E:%f.10", sh.a.en.y, sh.a.en.x );
    putText( draw, text, sh.a.xy + Point( 10, 10 ), CV_FONT_HERSHEY_COMPLEX_SMALL, 1.6, Scalar( 0, 255, 255 ), 2, 8 ); 

    circle( draw, sh.b.xy, 5, Scalar( 0, 255, 255 ), 2 );
    text = "B"; //format( "N:%f.10 E:%f.10", sh.b.en.y, sh.b.en.x );
    putText( draw, text, sh.b.xy + Point( 10, 10 ), CV_FONT_HERSHEY_COMPLEX_SMALL, 1.6, Scalar( 0, 255, 255 ), 2, 8 ); 

    //cout << setprecision(10);
    //cout << sh.sheet_name << " a.nord:" << sh.a.en.y << " a.east:" << sh.a.en.x << " raster:" << sh.a.xy << endl; 
    //cout << sh.sheet_name << " b.nord:" << sh.b.en.y << " b.east:" << sh.b.en.x << " raster:" << sh.b.xy << endl; 


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


int GeoMapEditor::finishMouseEvent() // ����� ���������, ���� ���������� � ��������� ��� ���������
{
  draw();
  update_title();
  return 0;
      //update_image_to_draw();
      //draw_image_dirty = true;
      //update_window(true);
      //return 0;
}


#define ADD_OBJECT_RECT      0x0001 // �������� �������� �������������
//#define ADD_OBJECT_SEGM      0x0002 // �������� �������� �������
//#define ADD_OBJECT_LINE      0x0004 // �������� �������� �����
//#define ADD_OBJECT_QUAD      0x0008  // �������� �������� ���������
//..

#define ADD_OBJECT_MOUSE_LEFT   0x0100 // ���������� ����� ������� ����
#define ADD_OBJECT_MOUSE_RIGHT  0x0200 // ���������� ������ ������� ����
#define ADD_OBJECT_MOUSE_SHIFT  0x0400 // ���������� � ������� SHIFT
#define ADD_OBJECT_MOUSE_CTRL   0x0800 // ���������� � ������� CONTROL

#define ADD_OBJECT_TRACKED      0x1000 // �������������� ���������� � ������ ��������


int GeoMapEditor::processMouseEvent(int event, int x, int y, int flags)
{

  //switch (mouseScenario)
  //{

  //}

  //cout << "event=" << event << " coords=" << x << " " << y << " flags=" << flags << endl;

  string objtype = objType(); // ��� �������, � ������� ��������
  bool object_poligonal = (objtype == "AFO_Segm" || objtype == "AFO_Triangle" || objtype == "AFO_Quad" || objtype == "AFO_Polygon");
  ///bool object_segment_based = (objtype == "AFO_Segm"); // ���� ����������� ���� �������� ���������������
  cv::Point curPoint(x,y);
  int max_poly_points=1000;
  if (objtype == "AFO_Segm")
    max_poly_points=2;
  if (objtype == "AFO_Triangle")
    max_poly_points=3;
  if (objtype == "AFO_Quad")
    max_poly_points=4;

  //cout << "GeoMap:objtype" << objtype << endl;

  switch (event)
  {
    case CV_EVENT_LBUTTONDOWN   : // =1,0,
    case CV_EVENT_RBUTTONDOWN   : // =1,0,
      if (object_poligonal)
      {
        if ( rubbering_pts.size() == 0 ) // ������ �����
        {
          rubbering_pts.push_back(curPoint);
          rubbering_pts.push_back(curPoint); // ������� ����������� ������ �������, ������ ����� ����� �������������� �� mousemove 
          rubbering_mode = RUBBERING_SEGMENT;
        }
        else if (rubbering_pts.size()>=2 && // ���� ��������� ������� �� ����������� -- ������ �����
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
        rubbering_pts.push_back(curPoint); // ������� ����������� �������� 
        rubbering_mode = RUBBERING_RECTANGLE; // ���� ���, �����.
      }
      rubber_by_left_button = (event == CV_EVENT_LBUTTONDOWN);
      return finishMouseEvent();

    case CV_EVENT_MOUSEMOVE     : // =0,
      if (rubbering_mode == RUBBERING_RECTANGLE)
      {
        assert(rubbering_pts.size() == 2);
        rubbering_pts[1] = curPoint; // �������� �����
        return finishMouseEvent();
      }
      if (rubbering_mode == RUBBERING_SEGMENT)
      {
        assert(rubbering_pts.size() >=2);
        ////if (rubbering_pts.size() == 2) 
          rubbering_pts.back() = curPoint; // �������� �����
        return finishMouseEvent();
      }
      if (rubbering_mode == RUBBERING_POLYGON) // ��� �������� ����� ��������������, � �� �������� ����� ������ �������� ������ ���������� ��������� ����� ��������������
      {
        assert(rubbering_pts.size() >=2);
        // ������ �� ������, ���� ����� ������������ �� ������� � ��������� �� ��������
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
        rubbering_pts[1] = curPoint; // ���������������
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
        if ( l2norm( rubbering_pts[0], curPoint) < 100 // �������� ��� ������ ���� ���. ����� �������
          //////||
          //////  (object_segment_based && rubbering_pts.size() == 2 )
          || rubbering_pts.size() == max_poly_points+1
            ) 
        {
          if (rubbering_pts.size() > 2)
            rubbering_pts.back() = rubbering_pts[0]; // ��������������� ��������� ����� �������� � ���������
          int flags = ADD_OBJECT_RECT | (rubber_by_left_button ? ADD_OBJECT_MOUSE_LEFT : ADD_OBJECT_MOUSE_RIGHT);
          if (l2norm( rubbering_pts[rubbering_pts.size()-2], rubbering_pts[rubbering_pts.size()-1] ) < 25)
          { // ������� ��������� ����� ���� ��� �����������
            rubbering_pts[rubbering_pts.size()-2] = rubbering_pts[0];
            rubbering_pts.pop_back();
          }
          rubbering_mode = RUBBERING_NOTHING;
          int res = addMouseObject(rubbering_pts, flags);
          rubbering_pts.clear();
          return res;
        }
        // �� ����������, ����������, ���� ������������
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

bool GeoMapEditor::addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������
    cv::Rect& rect, // note: in-out -- ������������ �������� �� ������� ������� �������� ��� ������� �������
    int flags )
{
  Point xy = center( rect );
  GeoSheet& sh = gm.sheets[ cur_sheet ];
  Point2d en = sh.xy2en( xy );
  AGM_Point* pp = new AGM_Point( en );
  gm.objects.push_back(cv::Ptr<AGM_Point>(pp));
  return true;
};


bool GeoMapEditor::addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������
  std::vector< cv::Point >& pts, // note: in-out -- ������������ ����� �� ������� ������� �������� ��� ������� �������
  int flags )
{
  return __false("GeoMapEditor::addMouseObject(std::vector< cv::Point >& pts) not implemented");
};