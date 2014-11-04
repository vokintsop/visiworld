CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT (jniInterface)

SET(JAVA_HOME $ENV{JAVA_HOME})
IF (NOT DEFINED JAVA_HOME)
    MESSAGE( "*******************************************************************")
    MESSAGE( "JAVA_HOME has to be set in the environment. Required to compile JNI")
    MESSAGE( "*******************************************************************")
    RETURN()
ENDIF (NOT DEFINED JAVA_HOME)

# Include frameproc.h
INCLUDE_DIRECTORIES(../streetglass)

INCLUDE_DIRECTORIES(.)

# We have JNI on Android as is.
IF(NOT ANDROID)
  # Find and setup JNI for linking of JNI DLL
  FIND_PACKAGE(JNI)

  # In case one compiles for some platform without JNI but has JAVA_HOME defined.
  IF (NOT JNI_FOUND)
    MESSAGE("*******************************************************************")
    MESSAGE("JNI was not found on your system.                                  ")
    MESSAGE("That is required to compile ${project}                             ")
    MESSAGE("*******************************************************************")
    RETURN()
  ENDIF(NOT JNI_FOUND)

  INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH})
  INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH2})
ENDIF(NOT ANDROID)

# Find and setup SWIG
IF(ANDROID)
  FIND_HOST_PACKAGE(SWIG REQUIRED)
ELSE()
  FIND_PACKAGE(SWIG REQUIRED)
ENDIF()

INCLUDE(${SWIG_USE_FILE})

# SWIG module in CMake does not know that dll library on Windows platform is RUNTIME target.
# As the result, CMake puts library into wrong directory. Here is a fix for this issue.
IF(WIN32)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}")
ENDIF(WIN32)

SET(MY_SWIG_FILE streetglass.i)

# Generate JAVA sources and JNI Dll source
SET_SOURCE_FILES_PROPERTIES(${MY_SWIG_FILE} PROPERTIES CPLUSPLUS ON)
# Set the java package of the resulting Java files
LIST(APPEND options -package com.swig -directors)
SET_SOURCE_FILES_PROPERTIES(${MY_SWIG_FILE} PROPERTIES SWIG_FLAGS "${options}")
SWIG_ADD_MODULE(jniInterface java ${MY_SWIG_FILE})

# link jni DLL against mock implementation
# change here for linking against real ocr engine or create 2nd JNI DLL
TARGET_LINK_LIBRARIES(jniInterface streetglass)
SWIG_LINK_LIBRARIES(jniInterface)


# Install the jni lib
INSTALL(TARGETS jniInterface
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION lib)

# Install all JAVA files in /bindings/java dir
INSTALL(
  CODE "FILE(GLOB java_files \"${CMAKE_CURRENT_BINARY_DIR}/*.java\")"
  CODE "FILE(INSTALL \${java_files} DESTINATION \${CMAKE_INSTALL_PREFIX}/bindings/java/com/swig)"
)
