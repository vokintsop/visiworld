IF( MSVC )
  PROJECT( correspond_keypoints )
  
  SET( SOURCES
    src/main.cpp 
    src/simpleframe.cpp
    src/correspond.cpp
  )

  SET( HEADERS
  	src/simpleframe.h
    src/init.h
    src/correspond.h
    src/rulers.h
  )

  ADD_EXECUTABLE( correspond_keypoints ${HEADERS} ${SOURCES} )
  TARGET_LINK_LIBRARIES( correspond_keypoints
    ${OpenCV_LIBS}
  )

ENDIF()