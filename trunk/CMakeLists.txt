CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( visiworld )
SET( root_dir ${CMAKE_CURRENT_SOURCE_DIR} )

# -----------------------------------------------------------------------
# Platform.

# Determine the platform
IF( WIN32 )
  SET( platform_name "win" )
ELSEIF( UNIX )
  SET( platform_name "unix" )
ELSE()
  MESSAGE( FATAL_ERROR "Unsupported platform." )
ENDIF()

# Determine platform bits.
IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
  SET( platform_bitness "32" )
ELSEIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set(platform_bitness "64")
ELSE()
  MESSAGE( FATAL_ERROR "Unsupported bitness." )
ENDIF()

# Platform is the pair of system and its bitness.
SET( platform "${platform_name}${platform_bitness}")
MESSAGE( "Platform is ${platform}" )
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# Configuration.
IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  SET( configuration "debug" )
ELSE()
  SET( configuration "release" )
ENDIF()
MESSAGE( "Configuration is ${configuration}" )
# -----------------------------------------------------------------------

# -----------------------------------------------------------------------
# C Runtime Environment.
IF( MSVC )
  SET( crt "vc" )
ELSEIF( CMAKE_COMPILER_IS_GNUCXX )
  SET( crt "gnu" )
ELSE()
  MESSAGE( FATAL_ERROR "Unsupported CRT." )
ENDIF()
# -----------------------------------------------------------------------


# -----------------------------------------------------------------------
# Setting output directories

# Runtime output directory.
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${root_dir}/bin.${platform}.${configuration}" )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${root_dir}/bin.${platform}.debug" )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${root_dir}/bin.${platform}.release" )

# Library output directory.
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY         "${root_dir}/bin.${platform}.${configuration}" )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${root_dir}/bin.${platform}.debug" )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${root_dir}/bin.${platform}.release" )

# Archive output directory.
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY         "${root_dir}/lib/${platform}.${crt}/${configuration}" )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${root_dir}/lib/${platform}.${crt}/debug" )
SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${root_dir}/lib/${platform}.${crt}/release" )
# -----------------------------------------------------------------------


# -----------------------------------------------------------------------
# Compiler flags.

# Directory with libraries link to.
# Here is a hack to set right link directory in MSVC IDE.
IF( MSVC_IDE )
  LINK_DIRECTORIES( "${root_dir}/lib/${platform}.${crt}/$(ConfigurationName)/" )
ELSE()
  LINK_DIRECTORIES( "${root_dir}/lib/${platform}.${crt}/${configuration}" )  
ENDIF()

# -----------------------------------------------------------------------
# Functions.


# -----------------------------------------------------------------------
# Include directories.
INCLUDE_DIRECTORIES(
  ${root_dir}/prj.core
  ${root_dir}/prj.sandbox
  ${root_dir}/prj.thirdparty)

# Compiler specific flags.
IF( MSVC )
  ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS)
#  SET(CMAKE_C_FLAGS "/Zp1 /W3 /nologo /wd4996")
  SET(CMAKE_C_FLAGS "/Zp1 /W3 /nologo")
  
  IF( ${configuration} MATCHES "debug" )
    SET( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "/LDd /MDd" )
  ELSE()
    SET( CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "/LD /MD" )
  ENDIF()
ENDIF()

IF( CMAKE_COMPILER_IS_GNUCXX )
  LINK_DIRECTORIES( ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
  ADD_DEFINITIONS(-D_REENTRANT)
  SET( CMAKE_C_FLAGS "-Wall -Wextra -Wcast-align -fPIC -Wno-unused -Wno-missing-field-initializers" )
  SET( CMAKE_INSTALL_RPATH "$ORIGIN" )
  SET( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )

  IF( ${configuration} MATCHES "debug" )
    SET( CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS} "-g" )
  ELSE()
    SET( CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -O4" )
  ENDIF()

  SET( CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS} )
ENDIF()
# -----------------------------------------------------------------------

find_package(OpenCV REQUIRED)

# -----------------------------------------------------------------------
# Add project collections to build tree.
ADD_SUBDIRECTORY( prj.core )
ADD_SUBDIRECTORY( prj.sandbox )
ADD_SUBDIRECTORY( prj.thirdparty )
# -----------------------------------------------------------------------
