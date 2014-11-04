#ifndef __MARKUP_H
#define __MARKUP_H

//#include <fstream>
//#include <string>
//#include <algorithm>
//#include <cassert>
//
//#include <iostream>
//#include <stdio.h>
//using namespace std;
//
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

#include <deque>

#include "streetglass/framedata.h" // ������, ������������ ��� ����������� �� �����
#include "streetglass/frameproc.h" // ��������� �����

using namespace cv;

enum Key {
  kEscape =27,  // �����
  kEnter =13, // ������� ���������� ������������ �� ����� ������� (������)
  kCtrlEnter =10, // ������� ���������� ������������ �� ����� ������� (_�����_)
  kBackSpace = 8, // �������� ���������� ������������ �� ����� �������
  kCtrlBackSpace=127, // ������������� ��� undo
  kCtrlZ=26, // ������������� ��� undo
  kLeftArrow =2424832, // ���������: �� ���� ������
  kRightArrow =2555904, // ���������: �� ���� �����
  kPageUp =2162688,
  kPageDown =2228224,
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

class AFOTypes // supported derived AFrameObject types
{
public:
  std::vector< string > objTypes;
  AFOTypes()
  { // ������ �������������� ������� ��������
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

class MarkupWindow
{
  std::string title; // ��������� (id)/(opencv ������������� ����)
  vector< MarkupAction > undo; // ���� ��� ������
  int iObjType; // ������ ��������� ���� ������� 
  std::deque< int > pressed_keys; // ������� �������������� ������
public:
  AFOTypes afoTypes; // ������� �������������� ���� ��������

  string objType() { return afoTypes.objTypes[iObjType]; }; // �������� ��� ������� 
private:
  // �������� -- ����� ������
  //...>> rubbering_pts[] << Point rubber_start, rubber_finish; // ������������ ������ �������������, �������
  bool rubber_by_left_button; // true ���� ������ ���������� _�����_ �������, ����� _������_
  //bool rubbering_rectangle; // true ���� � ������� ���������� ������ �������������
  int rubbering_mode; // ������ ���� �� ����� �� �����
#define RUBBERING_NOTHING 0
#define RUBBERING_RECTANGLE 1 // ���������� �������������
#define RUBBERING_SEGMENT 2 // ���������� ������� -- �� ����� ��� �� ��������� ������� �������
#define RUBBERING_POLYGON 3 // ��� �������� ����� ��������������, � �� �������� ����� ������ �������� ������ ���������� ��������� ����� ��������������

//#define RUBBERING_IMAGE 3 -- �������� �������� ��������������� ��������, �� �������
  std::vector< cv::Point > rubbering_pts; // ����������� �����

  bool non_stop_mode; // ��� ����� ����� ������� ������
  bool tracking_object; // ���������� � ���������� � non_stop_mode � ������ ��������������� ������ � ���������� �������
  bool track_forward; // true -- ������, ����� ����� (ctrl-enter)
  Rect tracked_rect; // ������������� ��� ������ �� ��������� �����

  Mat base_image; // background image
  Mat draw_image; // background image + marked objects
  bool draw_image_dirty; // ������������ �������� draw_image �� ������������� ����������� ���������
public:
  MarkupWindow( const char* title = "markup" );
  int process( string& _video_file_name, int start_frame=0 ); // ��������� ������

  FrameProc frameProc; // ���������� �����

  bool loadVideo( string& _video_file_name, int& start_frame ); // �������� ����������� � �������� ������ �������� 
  bool readFrame( int pos ); // ��������� ����������� ����

private: // video properties, initialized by loadVideo()
  VideoCapture video;
  string video_file_name;
  // video properties
  double fps; // = video.get( CV_CAP_PROP_FPS );
  int frames; // = int( video.get( CV_CAP_PROP_FRAME_COUNT ) );
  int frame_width; // = int( video.get( CV_CAP_PROP_FRAME_WIDTH ) );
  int frame_height; // = int( video.get( CV_CAP_PROP_FRAME_HEIGHT ) );
  Mat frame_image; // ����, �������� �� �����������
  int iframe; // ����� ��������������� ������
  int frame_time; // ����� ������ � ������������� �� ������ ������


public: 
  bool loadMarkupData( int& start_frame );
private: // markup data, initialized by loadMarkupData()
  string markup_filename;
  vector< FrameData > marked_frames; /// ��������������� � video <===> [iframe]


public:
  void setWindowText( const char* window_title );

  void update_window( bool quickly = false )  // _������_ ����� �������� ��������� � ������� ����������
  {
    if (draw_image_dirty)
    {
      imshow( title, draw_image );
      draw_image_dirty = false;
    }

    int key = waitKey( (non_stop_mode || quickly) ? 1 : 0);
    if (key != kNoKeyPressed)
      cout << "Key pressed " << key << endl;
    if (key != kNoKeyPressed && pressed_keys.size() < 1000)
      pressed_keys.push_back( key ); // �������� �� processEvents()

  }
  void drawRubbering(); // ��������� �������� ��������� ����������� ���������
  void update_image_to_draw()
  {
    draw_image = base_image.clone(); 
    drawObjects();
    drawRubbering();
    draw_image_dirty = true;
  }
  void setBaseImage( Mat& base )
  {
    base_image = base; 
    update_image_to_draw();
  }

  // mouse
  int processMouseEvent(int event, int x, int y, int flags);
  int finishMouseEvent(); // ����� ���������, ���� ���������� � ��������� ��� ���������

  int processKeys(); // �������� �� ������� ������� ������ � ������������
  void drawObjects(); // draws to draw_image
  int processEvents(); // returns navigation key

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

  bool addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������
    cv::Rect& rect, // note: in-out -- ������������ �������� �� ������� ������� �������� ��� ������� �������
    int flags );

  bool addMouseObject( // �������� �������� ����� ������ ������� ��� ������� ������
    std::vector< cv::Point >& pts, // note: in-out -- ������������ ����� �� ������� ������� �������� ��� ������� �������
    int flags );


  bool trackObject( // �������� �������� ����� ������ �������� ������ � �������� ����� 
    cv::Rect& rect, // note: in-out -- ������������ �������� �� ������� �������� ��� ������� �������
    int flags );

  //bool addMouseLineObject( // �������� �������� ����� ������ ������� ��� ������� ������
  //  cv::Point click_start, 
  //  cv::Point click_finish, 
  //  bool rubber_by_left_button);


  bool MarkupWindow::addObject( // ����� ���������� �������: �������� ��� ������ ������ ��� ������ ������ ������...
  const std::vector< cv::Point >& pts 
  , int flags
  );

  bool MarkupWindow::addObject( // ����� ���������� �������: �������� ��� ������ ������ ��� ������ ������ ������...
    const cv::Rect& rect // note: __in__ : �� ������ �������� ������� �� ��������
    , int flags
  )
  {
    std::vector< cv::Point > pts;
    pts.push_back( rect.tl() );
    pts.push_back( rect.br() );
    return addObject(pts, flags);    
  }


  bool adjustObjectRectangle( 
    cv::Rect& rect // �������� ��� ������ ������ ��� ������ ������ ������... -- ����� �������� �������������
    , int flags
  );

  void help(); // F1
  bool saveMarkup(); // F2
  bool saveFrameImage();// F3
  bool saveFrameObjectsImages(); // F4

  // visibank:
  bool saveFrameObjectImage( int iobj );
  bool deleteFrameObjectImage( int iobj ); // on undo
  std::string makeFrameObjectImageName( int iframe, const Rect& objRoi, int iobj, 
    const char* szObjType, bool ensureFolder = true );

}; // class MarkupWindow


int trackRectangle( cv::VideoCapture& cap, cv::Rect& rect, 
          int nframes, std::vector< cv::Rect >& result ); 


#endif // __MARKUP_H