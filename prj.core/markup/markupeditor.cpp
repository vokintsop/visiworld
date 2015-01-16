#include "ocvutils/precomp.h"

#include "markup/markupeditor.h"
#include "streetglass/frameproc.h" // обработка кадра

using namespace cv;


void MarkupEditor::drawRubbering()
{
  if (rubbering_mode == RUBBERING_SEGMENT || rubbering_mode == RUBBERING_POLYGON) // надо нарисовать предыдущие точки и ребра
    for (int i=0; i< int(rubbering_pts.size()); i++)
    {
      circle( draw_image, rubbering_pts[i], 5, Scalar( 255, 255, 0 ) );
      if (i>0)
        line( draw_image, rubbering_pts[i], rubbering_pts[i-1], Scalar( 255, 255, 0 ) );
      else if (rubbering_pts.size() >2)
        line( draw_image, rubbering_pts[0], rubbering_pts.back(), Scalar( 255, 255, 0 ) );
    }

  if (rubbering_mode == RUBBERING_RECTANGLE && rubbering_pts.size() == 2)
  {
    Rect rect( rubbering_pts[0], rubbering_pts[1] );
    rectangle( draw_image, rect, Scalar(0,250,0), 2 );
  }

}

void MarkupEditor::updateTitle()
{
  FrameData& frame_data = marked_frames[iframe];
  int numobj = frame_data.objects.size();
  string _title = format("Markup: %s [#%d of %d, %d msec]; type=%s objects on frame=%d sensitivity=%f tracking=%s", 
    video_file_name.c_str(),
    iframe, frames, frame_time, objType().c_str(), numobj, frameProc.sensitivity, tracking_object? "ON" : "OFF" );
  setWindowText( _title.c_str() );
}

void MarkupEditor::drawObjects()
{
  FrameData& frame_data = marked_frames[iframe];
  frameProc.draw( draw_image, objType() );
  for (int i=0; i< int( frame_data.objects.size() ); i++)
    frame_data.objects[i]->draw( draw_image );

  draw_image_dirty = true;
}

void mouseCallBack(int event, int x, int y, int flags, void *userdata)
{
  MarkupEditor* mw = (MarkupEditor*)userdata;
  mw->processMouseEvent(event, x, y, flags);
}

MarkupEditor::MarkupEditor( 
  bool iskitti,
  const char* title // = "markup" 
  ) :
  non_stop_mode(false), tracking_object(false),
    title(title), rubbering_mode(0), Markup(iskitti)
{
  iObjType = 1; // "RedManOverTimer";
  namedWindow( title, WINDOW_NORMAL ); //WINDOW_AUTOSIZE); // -- в режиме AUTOSIZE координаты x y мышки надо пересчитывать
  setMouseCallback( title, mouseCallBack, this);
};



int trackRectangle( cv::VideoCapture& cap, cv::Rect& rect, 
          int nframes, std::vector< cv::Rect >& result ); 


void MarkupEditor::help()
{
  cout << "Markup hot keys:" << endl;
  cout << "<F1> - help" << endl;
  cout << "<F2> - save video markup" << endl;
  cout << "<F3> - write image of current frame  to <videoname>.<#frame>.jpg" << endl;
  cout << "<F4> - write image of marked objects on the current frame to <videoname>.<#frame>.<#object>.jpg" << endl;
  cout << "*** Navigate:"  << endl;
  cout << "<Space> - pause/play"  << endl;
  cout << "<Left/RightArrow> - next/previous frame"  << endl;
  cout << "<PgUp/PgDn> - next/previous frames jump"  << endl;
  cout << "<Home/End> - first/last frames jump"  << endl;
  cout << "*** Edit:"  << endl;
  cout << "<Tab> - select object type to add" << endl;
  cout << "<Mouse-left-down> - start rubbering rectangle for object or start click inside pre-found" << endl;
  cout << "<Mouse-move> - while pressing left button - rubber new object rectangle" << endl;
  cout << "<Mouse-left-up> - finish rubbering rectangle for object or start click inside pre-found" << endl;
  cout << "<BackSpace> - delete last object added at the current frame" << endl;
  cout << "<Enter> - start/stop tracking mode for last last object added at the current frame" << endl;
  cout << "<Ctrl-D> - toggle on/off detailed visualization"  << endl;
  cout << "<//> or <*> - decrease/increase sensitivity"  << endl;

  // todo
}

int MarkupEditor::processKeys() // здесь обработка клавиш до выхода из цикла обработки данного кадра 
{
  while (1)
  {
    int key = kNoKeyPressed;
    if (pressed_keys.size() > 0)
    {
      key = pressed_keys.front(); // пока не извлекаем...
    }
    // здесь обработка клавиш до выхода из цикла обработки данного кадра 

    if (key == kBackSpace)
    {
      pressed_keys.pop_front();
      FrameData& frame_data = marked_frames[iframe];
      if (frame_data.objects.size()>0)
      {
        deleteFrameObjectImage(frame_data.objects.size()-1);
        frame_data.objects.pop_back();
      }
      update_image_to_draw();
      update_window();
      continue;
    }
    if (key == kF1)
    {
      pressed_keys.pop_front();
      help();
      continue;
    }
    if (key == kF2)
    {
      pressed_keys.pop_front();
      saveMarkupData();
      continue;
    }
    if (key == kF3)
    {
      pressed_keys.pop_front();
      saveFrameImage();
      continue;
    }
    if (key == kF4)
    {
      pressed_keys.pop_front();
      saveFrameObjectsImages();
      continue;
    }
    if (key == kTab)
    {
      pressed_keys.pop_front();
      iObjType = (iObjType+1)% afoTypes.objTypes.size();
      update_image_to_draw();
      update_window();
      continue;
    }

    if (key == kEnter || key == kCtrlEnter 
      || (key == kSpace && tracking_object) 
      ) // start-stop tracking
    {
      pressed_keys.pop_front();
      if (tracking_object) // stop tracking
      {
        tracking_object = false;
        non_stop_mode = false;
        update_image_to_draw();
        update_window();
        continue;
      }

      if (!non_stop_mode && !tracking_object) // start tracking
      { // start tracking mode
        if (key == kEnter)
          track_forward = true;
        else if (key == kCtrlEnter)
          track_forward = false;
        else
          assert(0); //третьего не дано

        FrameData& frame_data = marked_frames[iframe];
        if (frame_data.objects.size() > 0)
        {
          AFrameObject* afo = frame_data.objects.back(); // или взять последний добавленный в undo() ?
          non_stop_mode = true;
          tracked_rect = afo->rect;
          tracking_object = true;
          cout << "Start tracking " << afo->type << afo->rect << endl;
        }

        //vector< Rect > track_result;
        //trackRectangle( video, afo->rect, 20, track_result );
        // тут надо выгружать результаты track_result, 
        // но пока не дотянут файл разметки до MarkupEditor.
        // ...
        update_image_to_draw();
        //??update_window();
        continue;
      }
    } // (key == kEnter)
    return key; // неизвестная клавиша -- снаружи разберутся
  } // while (1)
  return kNoKeyPressed;
}

int MarkupEditor::processEvents()
{
  update_window();
  return processKeys();
}

void onTimer( double time );


int MarkupEditor::process( string& _video_file_path, int start_frame )
{
  video_file_path = _video_file_path;
  if (!initialize(_video_file_path, start_frame))
    return -1;

  // мы уже встали на нужный кадр start_frame:
  int last_frame = frames - 1;
  assert( iframe == start_frame );
  //iframe = -1000; ///  int( video.get( CV_CAP_PROP_POS_FRAMES  );


  int next_frame = start_frame; // номер фрейма, куда хотим встать, но еще не встали
  int key = kNoKeyPressed;

  while(1) // main loop
  {

    if (next_frame != iframe || frame_image.empty())
    {
      if (!readFrame(next_frame))
        break;

      if (!frameProc.process(frame_image))
        cout << "frameProc.process(frame_image) failed";

      update_image_to_draw();

      double msec = double(frame_time)/1000.;
      onTimer( msec );

      if (tracking_object)
      {
        FrameData& frame_data = marked_frames[iframe];
        if (trackObject( tracked_rect, ADD_OBJECT_RECT | ADD_OBJECT_TRACKED ) && frame_data.objects.size() > 0)
        {
          tracked_rect = frame_data.objects.back()->rect;
          update_window();
        }
        else
        {
          tracking_object = false;
          non_stop_mode = false;
          update_window();
        }
        //////////if (!track_forward)
        //////////  iframe -= 2;
      }
    }
    
    processEvents();

    if (pressed_keys.size() > 0)
    {
      cout << "Pressed keys queue:";
      for (int i=0; i<int(pressed_keys.size()); i++ )
        cout << " " << pressed_keys[i];
      cout << endl;
    };//    else    cout << "Pressed keys queue empty" << endl;

    key = kNoKeyPressed;
    if (pressed_keys.size() > 0)
    {
      key =  pressed_keys.front();
      pressed_keys.pop_front(); // в любом случае выталкиваем ее, даже если не знаем как обрабатывать
    }

    if (key == kEscape)
    {
      break;
    }
    if (key == kSpace) // frame by frame -- or nonstop
    {
      non_stop_mode = !non_stop_mode;
      continue;
    }
    if (key == kMultiply || key == kDivide)
    {
      frameProc.sensitivity = (key == kMultiply) ? frameProc.sensitivity * 1.05 : frameProc.sensitivity / 1.05;
      cout << "Sensitivity " << frameProc.sensitivity << endl;
      frame_image = cv::Mat(); // hmm
      assert( frame_image.empty() );
      continue;
    }
    if (key == kCtrl_D)
    {
      frameProc.detailed_visualization = !frameProc.detailed_visualization;
      cout << "frameProc.detailed_visualization " << frameProc.detailed_visualization << endl;
      frame_image = cv::Mat(); // hmm
      assert( frame_image.empty() );
      continue;
    }

    if (key == kPageDown)
    {
      non_stop_mode = false;
      if (frames-1 < iframe + 30)
        SoundUI( SUI_Bump );
      next_frame = min(frames-1, iframe + 30);
      continue;
    }
    if (key == kPageUp)
    {
      non_stop_mode = false;
      if (0 > iframe - 30)
        SoundUI( SUI_Bump );
      next_frame = max(0, iframe - 30);
      continue;
    }
    if (key == kHome)
    {
      non_stop_mode = false;
      next_frame = 0;
      continue;
    }
    if (key == kEnd)
    {
      non_stop_mode = false;
      next_frame = frames-1;
      continue;
    }

    if (key == kLeftArrow) // one frame back, paused
    {
      non_stop_mode = false;
      if (0 > iframe - 1)
        SoundUI( SUI_Bump );
      next_frame = max(0, iframe -1);
      continue;
    }
    if (key == kRightArrow) // one frame forward, paused
    {
      non_stop_mode = false;
      if (frames-1 < iframe + 1)
        SoundUI( SUI_Bump );
      next_frame = min(frames-1, iframe +1 );
      continue;
    }
    ////////////////////////////////////////
    // no key.. where to move?

    if (key != kNoKeyPressed)
      cout << "Key " << key << " skipped" << endl;

    if (non_stop_mode)
    {
      if (iframe < last_frame)
        next_frame = iframe+1; // direction??
      else
      {
        SoundUI( SUI_Bump );
        if (tracking_object)
          cout << "Last frame, so tracking stopped" << endl;
        tracking_object = false;
        non_stop_mode = false;
      }
    }


  } // main loop

  saveMarkupData(); // ask?

  return key;
}

void MarkupEditor::setWindowText( const char* window_title )
{
  set_window_text( title.c_str(), window_title );
}


///////////////////////////////////// visibank
bool MarkupEditor::initialize( string& _video_file_path, int& start_frame )
{
  if (!loadVideo(_video_file_path, start_frame)) //не обязательно видео
    return __false("Failed to load video");
  if (!loadMarkupData( start_frame ))
  {
    cout << "Failed to load markup" << endl;
    //return __false("Failed to load markup");
  }

  // go to start_frame position
  if (start_frame < 0)
    start_frame = 0;
  if (start_frame >= frames || frames <=0)
  {
    cout << "Requested start frame " << start_frame << " is out of range" << endl;
    return false;
  }
  if (!setCurFrame(start_frame))
  {
    cout << "Can't start from requested frame " << start_frame << endl;
    return false;
  }

  return true;
}


bool MarkupEditor::readSessionData( 
  cv::FileStorage& fs, 
  vector< FrameData >& frames,
  int& start_frame
  )
{
  if (!fs.isOpened())
    return __false("Attempt to read from not open storage");


  cv::FileNode last_session_node = fs["LastMarkupSession"];
  if (!last_session_node.empty())
  {
    if (start_frame < 0)
    try {  
      last_session_node["LastVisitedFrame"] >> start_frame;
    }
    catch (...)
    {
      cout << "no StartFrame specified" << endl;
      start_frame = -1;
    }

    string activeObjectType;
    last_session_node[ "ActiveObjectType" ] >> activeObjectType;
    if (!activeObjectType.empty()) // setup iObjType
      for (iObjType = int(afoTypes.objTypes.size()-1); iObjType >=0; iObjType-- ) // if not found -- set to 0
        if ( afoTypes.objTypes[iObjType] == activeObjectType )
          break;
  }
  return true;
}


bool MarkupEditor::writeSessionData( 
  cv::FileStorage& fs, 
  vector< FrameData >& frames,  
  int start_frame
  )
{
  if (!fs.isOpened())
    return __false("Attempt to write to not open storage");

  fs << "LastMarkupSession" << "{";
  fs << "LastVisitedFrame" << start_frame;
  fs << "ActiveObjectType" << objType();
  fs << "}";
  
  return true;
}

bool MarkupEditor::loadMarkupData( int& start_frame )
{
  ensure_folder( format( "%s.dat", video_file_path.c_str() ) );

  markup_filename = format( "%s.dat/%s.markup.xml",  
    video_file_path.c_str(),
    name_and_extension(video_file_path).c_str() );

  cv::FileStorage fs(markup_filename, cv::FileStorage::READ);
  if (!fs.isOpened())
  {
    marked_frames.resize( frames );
    return __false( format( "Failed to open storage to read %s", markup_filename.c_str() ) );
  }

  if (!readVideoData( fs, marked_frames ))
    marked_frames.resize( frames );

  if (!readSessionData( fs, marked_frames, start_frame ))
    marked_frames.resize( frames );

  return true;
}


bool MarkupEditor::saveMarkupData()  // F2
{
  ensure_folder( format( "%s.dat", video_file_path.c_str() ) );

  markup_filename = format( "%s.dat/%s.markup.xml",  
    video_file_path.c_str(),
    name_and_extension(video_file_path).c_str());

  cv::FileStorage fs(markup_filename, cv::FileStorage::WRITE);
  if (!fs.isOpened())
    return __false( format( "Failed to open storage to read %s", markup_filename.c_str() ) );

  writeVideoData( fs, marked_frames ); // todo -- check!!! try {} catch{{} for writeVideoData() ???
  writeSessionData( fs, marked_frames, iframe ); // todo -- check!!! try {} catch{{} for writeVideoData() ???
  cout << "Markup saved to: " << markup_filename << endl;
  return true; // todo -- check!!! try {} catch{{} for writeVideoData() ???
}

bool MarkupEditor::saveFrameImage() // F3
{
  string frame_name = format( "%s.%04d.jpg", video_file_path.c_str(), iframe );
  if (!imwrite( frame_name, frameProc.bgr720 )) // try catch?
    return __false( format("Write frame '%s' failed\n", frame_name.c_str() ) );
  cout << "Frame " << iframe << " saved to: " << frame_name << endl;
  return true; // todo -- check!!! try {} catch{{} for writeVideoData() ???
}




