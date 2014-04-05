IF( MSVC )
  PROJECT( testrun )
  
  SET( SOURCES
    ${CMAKE_SOURCE_DIR}/prj.core/cover_net/cover_net.h
    ../Hcoords.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h
    src/lines2points_2d.cpp
    src/points2lines_2d.cpp
    src/points2points_2d.cpp
    src/precomp.h
    src/main.cpp
    src/gen.h 
    )

  ADD_EXECUTABLE( testrun ${SOURCES} )
  TARGET_LINK_LIBRARIES( testrun
    ${OpenCV_LIBS}
  )

ENDIF()
