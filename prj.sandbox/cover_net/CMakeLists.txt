IF( MSVC )
  PROJECT( cover_net )
  
  SET( SOURCES
    src/cover_net.h
    src/ticker.h
    src/main.cpp 
    )

  ADD_EXECUTABLE( cover_net ${SOURCES} )
  TARGET_LINK_LIBRARIES( cover_net
    ${OpenCV_LIBS}
  )

ENDIF()
