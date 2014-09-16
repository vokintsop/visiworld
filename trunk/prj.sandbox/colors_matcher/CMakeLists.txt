IF( MSVC )
  PROJECT( colors_matcher )
  
  SET( SOURCES
    src/main.cpp 
    )

  ADD_EXECUTABLE( colors_matcher ${SOURCES} )
  TARGET_LINK_LIBRARIES( colors_matcher
    ${OpenCV_LIBS}
  )

ENDIF()
