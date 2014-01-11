IF( MSVC )
  PROJECT( letters )
  
  SET( SOURCES
    src/cover_tree.h
    src/main.cpp 
    )

  ADD_EXECUTABLE( letters ${SOURCES} )
  TARGET_LINK_LIBRARIES( letters
    ${OpenCV_LIBS}
  )

ENDIF()
