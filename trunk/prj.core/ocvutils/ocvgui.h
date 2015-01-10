#ifndef __OCVGUI_H
#define __OCVGUI_H

bool SetWindowText( const char* window_id, const char* window_text ); // устанавливает window_text в заголовок окна, зарегистрированного как window_id
int WaitKey( int delay ); // централизованный обработчик cv::waitKey(), копит клавиши в буфере, распределяет клавиши для обработки между окнами

class OCVGuiWindow 
  // окно, готовое: 
  // (1) принимать клавиши и отдавать на централизованное распределение (int OCVGuiWindow::waitKey( int delay ), 
  //     незаметно подменяя cv::waitKey(delay)
  // (2) принимать клавиши для обработки. Первым обрабатывает то окно, которое имело фокус ввода
{
  virtual int   waitKey( int delay ) { return WaitKey(delay); }; //перекрывает cv::waitKey(). однажды она вернется на procKey(), но возможно в другое окно
  virtual bool  procKey( int key )=0; // выполняет обработку клавиши для данного окна; возвращает true если клавиша обработана
};

#endif // __OCVGUI_H