IF( MSVC )
  PROJECT( simeion )
  
  SET( SOURCES
    src/cover_tree.h
    src/main.cpp 
    )

  ADD_EXECUTABLE( simeion ${SOURCES} )
  TARGET_LINK_LIBRARIES( simeion
    ${OpenCV_LIBS}
  )

ENDIF()
