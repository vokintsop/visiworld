IF( MSVC )
  PROJECT( testgen )
  
  SET( SOURCES
    src/lines2points_2d.cpp
    src/points2lines_2d.cpp
    src/points2points_2d.cpp
    src/precomp.h
    src/main.cpp
    src/gen.h 
    )

  ADD_EXECUTABLE( testgen ${SOURCES} )
  TARGET_LINK_LIBRARIES( testgen
    ${OpenCV_LIBS}
  )

ENDIF()
