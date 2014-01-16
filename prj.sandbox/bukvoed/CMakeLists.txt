IF( MSVC )
  PROJECT( bukvoed )
  
  SET( SOURCES
    src/pagedata.h
    src/pagedata.cpp
    src/precomp.h
    src/ccdata.h
    src/cover_net.h
    src/ticker.h
    src/bukvoed.cpp 
    src/main.cpp 
    src/niblack.h
    src/niblack.cpp
    )

  ADD_EXECUTABLE( bukvoed ${SOURCES} )
  TARGET_LINK_LIBRARIES( bukvoed ${OpenCV_LIBS} )

ENDIF()
