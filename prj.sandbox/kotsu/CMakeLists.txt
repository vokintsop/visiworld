IF( MSVC )
  PROJECT( kotsu )
  
  SET( SOURCES
    src/kotsu.cpp 
    )

  ADD_EXECUTABLE( kotsu ${SOURCES} )
  TARGET_LINK_LIBRARIES( kotsu
    ${OpenCV_LIBS}
  )

ENDIF()
