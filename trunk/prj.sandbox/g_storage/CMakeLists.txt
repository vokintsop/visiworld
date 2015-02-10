PROJECT( g_storage )
  
###  include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../prj.core/)
  
  SET( SOURCES

  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomap.h
  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomap.cpp
                     
  ${CMAKE_SOURCE_DIR}/prj.core/ocvutils/precomp.h

  ${CMAKE_SOURCE_DIR}/prj.core/soundui/soundui.h
  ${CMAKE_SOURCE_DIR}/prj.core/soundui/soundui.cpp
  
    src/gstorage.h
    src/gstorage.cpp
    src/main.cpp
    )

  ADD_EXECUTABLE( g_storage ${SOURCES} )
  TARGET_LINK_LIBRARIES( g_storage
    ${OpenCV_LIBS}
  #  jsoncpp
  )
    
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  