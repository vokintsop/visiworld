IF( MSVC )
  PROJECT( semeion )
  
  SET( SOURCES
    src/cover_tree.h
    src/main.cpp 
    )

  ADD_EXECUTABLE( semeion ${SOURCES} )
  TARGET_LINK_LIBRARIES( semeion
    ${OpenCV_LIBS}
  )

ENDIF()
