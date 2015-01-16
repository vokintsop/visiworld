#include <cmath>
#include <iostream>

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER
#include <soundui/soundui.h>

void SoundUI( int sui )
{
#ifdef _MSC_VER
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
#else // _MSC_VER
  return;
#endif // _MSC_VER
}
