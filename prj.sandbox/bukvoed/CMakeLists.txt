IF( MSVC )
  PROJECT( bukvoed )
  
  SET( SOURCES
    ${CMAKE_SOURCE_DIR}/prj.core/cover_net/cover_net.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h
    src/bukvoed.h
    src/pagedata.h
    src/pagedata.cpp
    src/precomp.h
    src/ccdata.h
    src/bukvoed.cpp 
    src/main.cpp 
    src/niblack.h
    src/niblack.cpp
    )

  ADD_EXECUTABLE( bukvoed ${SOURCES} )
  TARGET_LINK_LIBRARIES( bukvoed ${OpenCV_LIBS} )

ENDIF()
