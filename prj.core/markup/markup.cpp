#include "ocvutils/precomp.h"

#include "markup.h"
#include "streetglass/frameproc.h" // обработка кадра

using namespace cv;

Markup::Markup():
  fps(0),  frames(0),  frame_width(0),  frame_height(0)
{
}

bool Markup::readFrame( int pos )
{
  if (pos != iframe+1)
  {  // need jump
    if (!video.set( CV_CAP_PROP_POS_FRAMES,  pos ))
      return false;
  }
  frame_time = int(video.get( CV_CAP_PROP_POS_MSEC  )); // время текущего кадра 
  if (!video.read(frame_image)) 
  {
    cout << "Can't read requested frame " << pos << endl;
    return false;
  }
  iframe = pos;
  return true;
}

bool Markup::loadVideo( string& _video_file_path, int& start_frame )
{
  if (!video.open( _video_file_path ))
    return __false( format( "\nCan't open %s\n", _video_file_path.c_str() ) );

  video_file_path = _video_file_path;
  video_file_name = name_and_extension(video_file_path);

  ///video_short_name = 

  fps = video.get( CV_CAP_PROP_FPS );
  frames = int( video.get( CV_CAP_PROP_FRAME_COUNT ) );
  frame_width = int( video.get( CV_CAP_PROP_FRAME_WIDTH ) );
  frame_height = int( video.get( CV_CAP_PROP_FRAME_HEIGHT ) );
  cout << "Loaded video: " << video_file_path << endl;
  cout << "fps=" << fps << "\tframes=" << frames << endl;
  cout << "frame_width=" << frame_width << "\tframe_height=" << frame_height << endl;
}

bool Markup::readVideoData( 
  cv::FileStorage& fs, 
  vector< FrameData >& frames
  )
{
  if (!fs.isOpened())
    return __false("Attempt to read from not open storage");

  if (!readFrames( fs, frames ))
    return __false("Failed to readFrames");

  return true;
}


bool Markup::writeVideoData( 
  cv::FileStorage& fs, 
  vector< FrameData >& frames
  )
{
  if (!fs.isOpened())
    return __false("Attempt to write to not open storage");
  if (!writeFrames( fs, frames ))
    return __false("Failed to writeFrames()");
  return true;
}

std::string Markup::makeFrameObjectImageName( int iframe, const Rect& objRoi, int iobj, 
                                                   const char* szObjType, bool ensureFolder )
{
  ensure_folder( format("%s.dat", video_file_path.c_str() ) );
  ensure_folder( format("%s.dat/objects", video_file_path.c_str() ) );
  ensure_folder( format("%s.dat/objects/%s", video_file_path.c_str(), szObjType ) );

  string frame_obj_name = format( 
    "%s.dat/objects/%s/%s.frame%04d.x%04d.y%04d.w%04d.h%04d.obj%02d.png", 
    video_file_path.c_str(), szObjType, // videoname.dat/objects/objtype/, 
    name_and_extension(video_file_path).c_str(),
    iframe, objRoi.x, objRoi.y, objRoi.width, objRoi.height, iobj // videoname, frame, roi, index
     
    );
  return frame_obj_name;
}

bool Markup::deleteFrameObjectImage( int iobj )
{
  FrameData& fd = marked_frames[iframe];
  Rect rc = fd.objects[iobj]->rect;
  rc.x-= rc.width/2;
  rc.y-= rc.height/2;
  rc.width *=2;
  rc.height *=2;
  string frame_obj_name = makeFrameObjectImageName( iframe, rc, iobj, fd.objects[iobj]->type.c_str(), false );
    //format( "%s.frame%04d.x%04d.y%04d.w%04d.h%04d.obj%02d.%s.png", 
    //video_file_path.c_str(), iframe, rc.x, rc.y, rc.width, rc.height, iobj, 
    //fd.objects[iobj]->type.c_str() 
    //);
  _unlink( frame_obj_name.c_str() );
  cout << "Frame object " << iobj << " file " << frame_obj_name << " is deleted" << endl;

  return true;
}

bool Markup::saveFrameObjectImage( int iobj )
{
  FrameData& fd = marked_frames[iframe];
  Rect rc = fd.objects[iobj]->rect;
  Rect base = cv::Rect( Point(0,0), frame_image.size());
  int dw = max( 5, min( 20, rc.width/2 ));
  int dh = max( 5, min( 20, rc.height/2 ));

  rc.x-= dw; //// todo: обработка случаев когда объект выходит за границы картинки...
  rc.y-= dh;
  rc.width  +=dw*2;
  rc.height +=dh*2;

  rc &= base;

  string frame_obj_name = makeFrameObjectImageName( iframe, rc, iobj, fd.objects[iobj]->type.c_str(), true );
    //format( "%s.frame%04d.x%04d.y%04d.w%04d.h%04d.obj%02d.%s.png", 
    //video_file_path.c_str(), iframe, rc.x, rc.y, rc.width, rc.height, iobj, 
    //fd.objects[iobj]->type.c_str() 
    //);
  try {
  Mat object_image = frame_image(rc);
  Rect rc2 = fd.objects[iobj]->rect; 
  rc2.x -= rc.x;
  rc2.y -= rc.y;

  rectangle( object_image, rc2, Scalar( 255, 0, 255, 0 ), 1 );

  if (!imwrite( frame_obj_name, object_image )) // try catch?
    return __false( format("Write object image '%s' failed\n", frame_obj_name.c_str() ) );
  cout << "Frame object " << iobj << " saved to: " << frame_obj_name << endl;
  }
  catch (...) {
    cout << "Frame object " << iobj << " with rect " << rc << "NOT saved to: " << frame_obj_name << endl;
    return false;
  }
  return true;
}

bool Markup::saveFrameObjectsImages() // F4
{
  FrameData& fd = marked_frames[iframe];
  //exportFrameData( cv::Mat& base_image, std::string& videofile, int iframe )
  for (int iobj=0; iobj<int(fd.objects.size()); iobj++ )
    if (!saveFrameObjectImage(iobj))
      return false;

  return true; // todo -- check!!! try {} catch{{} for writeVideoData() ???
}


