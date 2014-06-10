IF( MSVC )
  PROJECT( calib_with_correspondence )
  
  SET( SOURCES
    src/main.cpp 
    src/simpleframe.cpp
    src/triangulation.cpp
	src/calibration.cpp
  )

  SET( HEADERS
  	src/simpleframe.h
    src/init.h
    src/rulers.h
    src/triangulation.h
	src/calibration.h
  )

  ADD_EXECUTABLE( calib_with_correspondence ${HEADERS} ${SOURCES} )
  TARGET_LINK_LIBRARIES( calib_with_correspondence
    ${OpenCV_LIBS}
  )

ENDIF()