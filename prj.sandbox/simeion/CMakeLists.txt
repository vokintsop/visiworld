IF( MSVC )
  PROJECT( simeion )
  
  SET( SOURCES
    src/main.cpp 
    )

  ADD_EXECUTABLE( simeion ${SOURCES} )
  TARGET_LINK_LIBRARIES( simeion
    ${OpenCV_LIBS}
  )

ENDIF()
