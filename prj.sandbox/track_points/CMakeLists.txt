IF( MSVC )
  PROJECT( track_points )
  
  SET( SOURCES
    src/main.cpp 
    src/procframe.cpp 
    )

  ADD_EXECUTABLE( track_points ${SOURCES} )
  TARGET_LINK_LIBRARIES( track_points
    ${OpenCV_LIBS}
  )

ENDIF()
