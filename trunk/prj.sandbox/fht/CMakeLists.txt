IF( MSVC )
  PROJECT( fht )
  
  SET( SOURCES
    src/fht.cpp 
    src/fht.h
    src/main.cpp
    )

  ADD_EXECUTABLE( fht ${SOURCES} )
  TARGET_LINK_LIBRARIES( fht
    ${OpenCV_LIBS}
  )

ENDIF()
