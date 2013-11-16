IF( MSVC )
  PROJECT( mnist )
  
  SET( SOURCES
    src/cover_tree.h
    src/main.cpp 
    )

  ADD_EXECUTABLE( mnist ${SOURCES} )
  TARGET_LINK_LIBRARIES( mnist
    ${OpenCV_LIBS}
  )

ENDIF()
