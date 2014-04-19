IF( MSVC )
  PROJECT( yorkurbandb )
  
  SET( SOURCES
    ${CMAKE_SOURCE_DIR}/prj.core/cover_net/cover_net.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/hcoords.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ocvkeys.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ocvgui.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ocvgui.cpp
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h
    src/explore.cpp 
    src/imagerecord.h 
    src/main.cpp 
    src/read.cpp 
    src/report.cpp 
    )

  ADD_EXECUTABLE( yorkurbandb ${SOURCES} )
  TARGET_LINK_LIBRARIES( yorkurbandb
    ${OpenCV_LIBS}
  )

ENDIF()
