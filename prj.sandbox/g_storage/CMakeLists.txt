PROJECT( g_storage )

###  include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../prj.core/)

  SET( SOURCES_LIB
  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomap.h
  ${CMAKE_SOURCE_DIR}/prj.core/geomap/geomap.cpp
    src/gstorage.h
    src/gstorage.cpp
    )

  ADD_LIBRARY( g_storage ${SOURCES_LIB} )
  TARGET_LINK_LIBRARIES( g_storage
    ${OpenCV_LIBS}
  #  jsoncpp
  )

  SET( SOURCES_GO
  ${CMAKE_SOURCE_DIR}/prj.core/soundui/soundui.h
  ${CMAKE_SOURCE_DIR}/prj.core/soundui/soundui.cpp
  
    src/gstorage.h
    src/main.cpp
    )

  ADD_EXECUTABLE( g_storage_go ${SOURCES_GO} )
  TARGET_LINK_LIBRARIES( g_storage_go
       g_storage
      )
  add_dependencies(g_storage_go 
       g_storage)

  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  