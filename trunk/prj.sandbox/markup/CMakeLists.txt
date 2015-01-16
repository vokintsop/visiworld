cmake_minimum_required(VERSION 2.8)
find_package( OpenCV REQUIRED )

PROJECT( markup )

SET( SOURCES
  ${CMAKE_SOURCE_DIR}/prj.core/binarize/niblack.h
  ${CMAKE_SOURCE_DIR}/prj.core/binarize/niblack.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/conncomp/ccdata.h
  ${CMAKE_SOURCE_DIR}/prj.core/conncomp/ccmap.h
  ${CMAKE_SOURCE_DIR}/prj.core/conncomp/ccmap.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/detect/detect_segment.h
  ${CMAKE_SOURCE_DIR}/prj.core/detect/detect_segment.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomap.h
  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomap.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomapeditor.h
  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomapeditor.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/gnss/gnss.h
  ${CMAKE_SOURCE_DIR}/prj.core/gnss/gnss.cpp

  ${CMAKE_SOURCE_DIR}/prj.core/markup/markup.h
  ${CMAKE_SOURCE_DIR}/prj.core/markup/markup.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/markup/markupeditor.h
  ${CMAKE_SOURCE_DIR}/prj.core/markup/markupeditor.cpp

  ${CMAKE_SOURCE_DIR}/prj.core/markup/markup_addobject.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/markup/markup_mouse.cpp

  ${CMAKE_SOURCE_DIR}/prj.core/markup/kitti.h
  ${CMAKE_SOURCE_DIR}/prj.core/markup/kitti.cpp

  ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ocvgui.h
  ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ocvgui.cpp

  ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ocvkeys.h
  ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ocvutils.h
  ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/precomp.h
  ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h

  ${CMAKE_SOURCE_DIR}/prj.core/soundui/soundui.h
  ${CMAKE_SOURCE_DIR}/prj.core/soundui/soundui.cpp

  ${CMAKE_SOURCE_DIR}/prj.core/streetglass/framedata.h
  ${CMAKE_SOURCE_DIR}/prj.core/streetglass/frameproc.h
  ${CMAKE_SOURCE_DIR}/prj.core/streetglass/frameproc.cpp
  ${CMAKE_SOURCE_DIR}/prj.core/streetglass/sticks.h
  ${CMAKE_SOURCE_DIR}/prj.core/streetglass/sticks.cpp

  src/main.cpp
  src/track.cpp
  )

include_directories(/ ${CMAKE_SOURCE_DIR})

ADD_EXECUTABLE( markup  ${SOURCES} )
TARGET_LINK_LIBRARIES( markup  ${OpenCV_LIBS}  )
