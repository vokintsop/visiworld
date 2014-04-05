IF( MSVC )
  PROJECT( cover_net )
  
  SET( SOURCES
    ${CMAKE_SOURCE_DIR}/prj.core/cover_net/cover_net.h
    ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/ticker.h
    src/main.cpp 
    src/mnist.cpp 
    src/words.cpp 
    src/test.h

    src/graph.h
    src/simpletest.h
    )

  ADD_EXECUTABLE( cover_net ${SOURCES} )
  TARGET_LINK_LIBRARIES( cover_net
    ${OpenCV_LIBS}
  )

ENDIF()
