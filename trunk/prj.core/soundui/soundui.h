#ifndef __SOUNDUI_H
#define __SOUNDUI_H

#define SUI_Beep  0 // короткий нежный звучок
#define SUI_Alert 1 // предупреждение о мелкой поломке
#define SUI_Ok    2 // успешное завершение длительного процесса
#define SUI_Bump  3 // средней продолжительности, воткнулись в препятствие

void SoundUI( int sui=0 );


#endif //__SOUNDUI_H