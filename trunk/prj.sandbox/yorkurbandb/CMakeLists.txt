IF( MSVC )
  PROJECT( yorkurbandb )
  
  SET( SOURCES
    ../cover_net.h
    src/explore.cpp 
    src/imagerecord.h 
    src/main.cpp 
    src/read.cpp 
    src/report.cpp 
    )

  ADD_EXECUTABLE( yorkurbandb ${SOURCES} )
  TARGET_LINK_LIBRARIES( yorkurbandb
    ${OpenCV_LIBS}
  )

ENDIF()
