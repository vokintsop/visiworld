IF( MSVC )
  PROJECT( overcom )
  
  SET( SOURCES
    src/main.cpp 
    )

  ADD_EXECUTABLE( overcom ${SOURCES} )
  TARGET_LINK_LIBRARIES( overcom
    ${OpenCV_LIBS}
  )

ENDIF()
