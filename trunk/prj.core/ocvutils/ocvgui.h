#ifndef __OCVGUI_H
#define __OCVGUI_H

bool SetWindowText( const char* window_id, const char* window_text ); // ������������� window_text � ��������� ����, ������������������� ��� window_id
int WaitKey( int delay ); // ���������������� ���������� cv::waitKey(), ����� ������� � ������, ������������ ������� ��� ��������� ����� ������

class OCVGuiWindow 
  // ����, �������: 
  // (1) ��������� ������� � �������� �� ���������������� ������������� (int OCVGuiWindow::waitKey( int delay ), 
  //     ��������� �������� cv::waitKey(delay)
  // (2) ��������� ������� ��� ���������. ������ ������������ �� ����, ������� ����� ����� �����
{
  virtual int   waitKey( int delay ) { return WaitKey(delay); }; //����������� cv::waitKey(). ������� ��� �������� �� procKey(), �� �������� � ������ ����
  virtual bool  procKey( int key )=0; // ��������� ��������� ������� ��� ������� ����; ���������� true ���� ������� ����������
};

#endif // __OCVGUI_H