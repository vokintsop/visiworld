IF( MSVC )
  PROJECT( yorkurbandb )
  
  SET( SOURCES
    ../cover_net.h
    src/main.cpp 
    )

  ADD_EXECUTABLE( yorkurbandb ${SOURCES} )
  TARGET_LINK_LIBRARIES( yorkurbandb
    ${OpenCV_LIBS}
  )

ENDIF()
