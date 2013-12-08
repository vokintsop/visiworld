IF( MSVC )
  PROJECT( bukvoed )
  
  SET( SOURCES
    src/cover_net.h
    src/ticker.h
    src/bukvoed.cpp 
    src/main.cpp 
    )

  ADD_EXECUTABLE( bukvoed ${SOURCES} )
  TARGET_LINK_LIBRARIES( bukvoed ${OpenCV_LIBS} )

ENDIF()
