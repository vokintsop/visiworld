IF( MSVC )
  PROJECT( testgen )
  
  SET( SOURCES
    ${CMAKE_SOURCE_DIR}/prj.core/cover_net/cover_net.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/hcoords.h
    src/lines2points_2d.cpp
    src/points2lines_2d.cpp
    src/points2points_2d.cpp
    src/points3d_line_vconst.cpp
    src/precomp.h
    src/main.cpp
    src/gen.h 
    )

  ADD_EXECUTABLE( testgen ${SOURCES} )
  TARGET_LINK_LIBRARIES( testgen
    ${OpenCV_LIBS}
  )

ENDIF()
