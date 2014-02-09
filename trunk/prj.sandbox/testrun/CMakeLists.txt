IF( MSVC )
  PROJECT( testrun )
  
  SET( SOURCES
    ../cover_net.h
    ../ticker.h
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
