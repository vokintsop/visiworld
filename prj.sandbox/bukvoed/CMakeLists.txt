IF( MSVC )
  PROJECT( bukvoed )
  
  SET( SOURCES
    ${CMAKE_SOURCE_DIR}/prj.core/cover_net/cover_net.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h
    src/bookdata.h
    src/bookdata.cpp
    src/bukvoed.h
    src/bukvoed.cpp 
    src/ccdata.h
    src/main.cpp 
    src/niblack.h
    src/niblack.cpp
    src/pagedata.h
    src/pagedata.cpp
    src/precomp.h
    )

  ADD_EXECUTABLE( bukvoed ${SOURCES} )
  TARGET_LINK_LIBRARIES( bukvoed ${OpenCV_LIBS} )

ENDIF()
