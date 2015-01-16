#include <cmath>
#include <iostream>
#include <windows.h>
#include <soundui/soundui.h>

void SoundUI( int sui )
{
  switch (sui)
  {
    case SUI_Alert:       
      Beep(440, 1000); break;
    case SUI_Ok:          
      Beep(220, 500); break;
    case SUI_Bump:        
      Beep(220*sqrt(2.), 250); break;

    case SUI_Beep:          
    default :
       Beep(110, 250); 
  }
}
