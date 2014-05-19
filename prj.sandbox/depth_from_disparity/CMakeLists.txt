IF( MSVC )
  PROJECT( depth_from_disparity )
  
  SET( SOURCES
    src/main.cpp 
    src/simpleframe.cpp
    src/triangulation.cpp
  )

  SET( HEADERS
  	src/simpleframe.h
    src/init.h
    src/rulers.h
    src/triangulation.h
  )

  ADD_EXECUTABLE( depth_from_disparity ${HEADERS} ${SOURCES} )
  TARGET_LINK_LIBRARIES( depth_from_disparity
    ${OpenCV_LIBS}
  )

ENDIF()