#ifndef __MARKUP_H
#define __MARKUP_H

#include <deque>
#include <string>

#include "streetglass/framedata.h" // данные, распознанные или размеченные на кадре
#include "streetglass/frameproc.h" // обработка кадра

class Markup
{
protected:
  Markup();
  std::vector< FrameData > marked_frames; /// синхронизирован с video <===> [iframe]

///////////////////////////////////// video -- объект разметки
protected:
  bool loadVideo( std::string& _video_file_name, int& start_frame ); // открытие видеопотока и загрузка данных разметки 
  bool readFrame( int pos ); // считываем запрошенный кадр

protected: // video properties, initialized by loadVideo()
  cv::VideoCapture video;
  std::string video_file_path; // full path+name
  std::string video_file_name; // name+extension
  // video properties
  double fps; // = video.get( CV_CAP_PROP_FPS );
  int frames; // = int( video.get( CV_CAP_PROP_FRAME_COUNT ) );
  int frame_width; // = int( video.get( CV_CAP_PROP_FRAME_WIDTH ) );
  int frame_height; // = int( video.get( CV_CAP_PROP_FRAME_HEIGHT ) );
  cv::Mat frame_image; // кадр, принятый из видеопотока
  int iframe; // номер обрабатываемого фрейма
  int frame_time; // время фрейма в миллисекундах от начала ролика

//////////////////////////////////////////////// persistence
protected:
  std::string markup_filename; // file based

public: 

  bool readVideoData( 
    cv::FileStorage& fs, 
    std::vector< FrameData >& frames
    );
  bool writeVideoData( 
    cv::FileStorage& fs, 
    std::vector< FrameData >& frames
    );

  bool saveFrameObjectsImages(); // F4

  // visibank:
  bool saveFrameObjectImage( int iobj );
  bool deleteFrameObjectImage( int iobj ); // on undo
  std::string makeFrameObjectImageName( int iframe, const cv::Rect& objRoi, int iobj, 
    const char* szObjType, bool ensureFolder = true );
};


int trackRectangle( cv::VideoCapture& cap, cv::Rect& rect, 
          int nframes, std::vector< cv::Rect >& result ); 


#endif // __MARKUP_H