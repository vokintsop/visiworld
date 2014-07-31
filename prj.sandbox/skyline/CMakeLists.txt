IF( MSVC )
  PROJECT( skyline )
  
  SET( SOURCES
    src/main.cpp 
    )

  ADD_EXECUTABLE( skyline ${SOURCES} )
  TARGET_LINK_LIBRARIES( skyline
    ${OpenCV_LIBS}
  )

ENDIF()
