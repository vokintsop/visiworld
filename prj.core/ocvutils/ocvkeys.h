#ifndef __OCVKEYS_H
#define __OCVKEYS_H

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
  kMultiply = 42, // asterick, "increase"
  kDivide = 47, // back slash, "decrease"
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

#endif // __OCVKEYS_H