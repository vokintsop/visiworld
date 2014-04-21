IF( MSVC )
  PROJECT( feature_sandbox )
  
  SET( SOURCES
    src/main.cpp 
    src/simpleframe.cpp
  )

  SET( HEADERS
  	src/simpleframe.h
    src/keypointruler.h
    ${CMAKE_SOURCE_DIR}/prj.core/cover_net/cover_net.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/hcoords.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h
  )

  ADD_EXECUTABLE( feature_sandbox ${HEADERS} ${SOURCES} )
  TARGET_LINK_LIBRARIES( feature_sandbox
    ${OpenCV_LIBS}
  )

ENDIF()