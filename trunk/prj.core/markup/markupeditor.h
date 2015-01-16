#ifndef __MARKUP_EDITOR
#define __MARKUP_EDITOR

#include <markup/markup.h>

using namespace cv;
#include "ocvutils/ocvkeys.h"
#include "ocvutils/ocvgui.h"

/****
enum Key {
  kEscape =27,  // выход
  kEnter =13, // трекинг последнего добавленного на кадре объекта (вперед)
  kCtrlEnter =10, // трекинг последнего добавленного на кадре объекта (_назад_)
  kBackSpace = 8, // удаление последнего добавленного на кадре объекта
  kCtrlBackSpace=127, // резервировано для undo
  kCtrlZ=26, // резервировано для undo
  kLeftArrow =2424832, // навигация: на кадр вперед
  kRightArrow =2555904, // навигация: на кадр назад
  kPageUp =2162688,
  kPageDown =2228224,
  kHome =2359296,
  kEnd = 2293760,
  kSpace =32, // nonstop/pause
  kTab =9, // select object type
  kGreyPlus =43,
  kGreyMinus =45,
  kW =119,
  kA =97,
  kS =115,
  kD =100,
  kWrus =246,
  kArus =244,
  kSrus =251,
  kDrus =226,
  kPlus =61,
  kMinus =45,

  kF1 = 7340032, // help
  kF2 = 7405568, // save markup
  kF3 = 7471104, // write image of current frame  to <videoname>.<#frame>.jpg
  kF4 = 7536640, // write image of marked objects on the current frame to <videoname>.<#frame>.<#object>.jpg


  kNoKeyPressed =-1  // after positive delay no key pressed -- process next image
};
*/



class AFOTypes // supported derived AFrameObject types
{
public:
  std::vector< string > objTypes;
  AFOTypes()
  { // список поддерживаемых классов объектов
    objTypes.push_back( "AFO_Unknown" );
    objTypes.push_back( "AFO_Point" );
    objTypes.push_back( "AFO_Segm" );
    objTypes.push_back( "AFO_Rect" );
    objTypes.push_back( "AFO_Quad" );
    objTypes.push_back( "AFO_Triangle" );


    objTypes.push_back( "AFO_RedStopLights" );
    objTypes.push_back( "AFO_RedManSingle" );
    objTypes.push_back( "AFO_GreenManSingle" );
    //objTypes.push_back( "AFO_RedManOverTimer" );
    objTypes.push_back( "AFO_GreenManBelowTimer" );
    objTypes.push_back( "AFO_BlueSquare" );
  }
};

class MarkupAction
{
  int action;
#define MA_ADD_OBJECT 1
#define MA_START_OBJECT_TRACKING 2
  int iframe;
  int iobject;
public:
  MarkupAction( int action, int iframe=-1, int iobject=-1 ):
  action(action), iframe(iframe), iobject(iobject){}
};


class MarkupEditor 
    : public OCVGuiWindow
    , public Markup // todo: сделать членом класса а не выводить

{
/////////////////////////// supported object types
  int iObjType; // индекс активного типа объекта 
public:
  AFOTypes afoTypes; // текущие поддерживаемые типы объектов
  string objType() { return afoTypes.objTypes[iObjType]; }; // активный тип объекта 


  std::string title; // заголовок (id)/(opencv идентификатор окна)
  vector< MarkupAction > undo; // стек для отката
  std::deque< int > pressed_keys; // очередь необработанных клавиш

  bool initialize( string& _video_file_path, int& start_frame );


  bool loadMarkupData( int& start_frame );
  bool saveMarkupData(); // F2
  bool saveFrameImage();// F3

  bool readSessionData(
    cv::FileStorage& fs, 
    vector< FrameData >& frames,
    int& start_frame
    );
  bool writeSessionData( 
    cv::FileStorage& fs, 
    vector< FrameData >& frames,  
    int start_frame
    );
private:
  // подрежим -- тянем мышкой
  //...>> rubbering_pts[] << Point rubber_start, rubber_finish; // вытягиваемый мышкой прямоугольник, отрезок
  bool rubber_by_left_button; // true если начали вытягивать _левой_ кнопкой, иначе _правой_
  //bool rubbering_rectangle; // true если в моменте вытягиваем мышкой прямоугольник
  int rubbering_mode; // какого кота мы тянем за хвост
#define RUBBERING_NOTHING 0
#define RUBBERING_RECTANGLE 1 // вытягиваем прямоугольник
#define RUBBERING_SEGMENT 2 // вытягиваем отрезок -- от точки или от последней вершины ломаной
#define RUBBERING_POLYGON 3 // уже вытянута часть многоугольника, и мы свободно водим мышкой планируя начать вытягивать следующее ребро многоугольника

//#define RUBBERING_IMAGE 3 -- например пытаемся отскроллировать картинку, на будущее
  std::vector< cv::Point > rubbering_pts; // накопленные точки

  bool non_stop_mode; // как долго ждать нажатия клавиш
  bool tracking_object; // включается в дополнение к non_stop_mode в режиме автоматического поиска и добавления объекта
  bool track_forward; // true -- вперед, иначе назад (ctrl-enter)
  Rect tracked_rect; // прямоугольник для поиска на следующем кадре

  //Mat base_image; // background image
  Mat draw_image; // background image + marked objects
  bool draw_image_dirty; // отрисованная картинка draw_image не соответствует внутреннему состоянию
public:
  MarkupEditor( bool iskitti = false, const char* title = "markup1" );
  int process( string& _video_file_name, int start_frame=0 ); // обработка ролика

  FrameProc frameProc; // обработчик кадра

  cv::Mat GetBaseImageCopy() const
  {
    return draw_image.clone();
  }

public:
  int procKey();

  void setWindowText( const char* window_title );
  void update_window( bool quickly = false )  // _только_ здесь реальная отрисовка с отдачей управления
  {
    if (draw_image_dirty)
    {
      imshow( title, draw_image );
      draw_image_dirty = false;
    }
    updateTitle();

    int key = kNoKeyPressed;
    if (pressed_keys.size() == 0)      
      key = waitKey( (non_stop_mode || quickly) ? 1 : 0);
    if (key != kNoKeyPressed)
      cout << "Key pressed " << key << endl;
    if (key != kNoKeyPressed && pressed_keys.size() < 1000)
      pressed_keys.push_back( key ); // отложено до processEvents()

  }
  void drawRubbering(); // отрисовка текущего состояния вытягивания недоноска
  void update_image_to_draw()
  {
    draw_image = frame_image.clone(); 
    drawObjects();
    drawRubbering();
    updateTitle();
    draw_image_dirty = true;
  }

  //void setBaseImage( Mat& base )
  //{
  //  base_image = base; 
  //  update_image_to_draw();
  //}

  void updateTitle(); // обновляет информационную строку в заголовке
  // mouse
  int processMouseEvent(int event, int x, int y, int flags);
  int finishMouseEvent(); // евент обработан, надо отрисовать и закончить его обработку

  int processKeys(); // вынимает из очереди нажатых клавиш и обрабатывает
  void drawObjects(); // draws to draw_image
  int processEvents(); // returns navigation key

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

  bool addMouseObject( // пытаемся добавить новый объект вытянув или кликнув мышкой
    cv::Rect& rect, // note: in-out -- подкручиваем ректангл по законам первого рождения для данного объекта
    int flags );

  bool addMouseObject( // пытаемся добавить новый объект вытянув или кликнув мышкой
    std::vector< cv::Point >& pts, // note: in-out -- подкручиваем точки по законам первого рождения для данного объекта
    int flags );


  bool trackObject( // пытаемся добавить новый объект протащив старый с прежнего кадра 
    cv::Rect& rect, // note: in-out -- подкручиваем ректангл по законам трекинга для данного объекта
    int flags );

  //bool addMouseLineObject( // пытаемся добавить новый объект вытянув или кликнув мышкой
  //  cv::Point click_start, 
  //  cv::Point click_finish, 
  //  bool rubber_by_left_button);


  bool addObject( // финал добавления объекта: кликнули или мышкой обвели или тречим старый объект...
  const std::vector< cv::Point >& pts 
  , int flags
  );

  bool addObject( // финал добавления объекта: кликнули или мышкой обвели или тречим старый объект...
    const cv::Rect& rect // note: __in__ : на выходе ректангл объекта не меняется
    , int flags
  )
  {
    std::vector< cv::Point > pts;
    pts.push_back( rect.tl() );
    pts.push_back( rect.br() );
    return addObject(pts, flags);    
  }


  bool adjustObjectRectangle( 
    cv::Rect& rect // кликнули или мышкой обвели или тречим старый объект... -- хотим уточнить прямоугольник
    , int flags
  );

  void help(); // F1

}; // class MarkupEditor

#endif // __MARKUP_EDITOR 
