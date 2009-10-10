# cmake build configuration for libMini
# (c) by Stefan Roettger

OPTION(BUILD_MINI_STUB "Build stub library." OFF)

OPTION(BUILD_MINI_WITHOUT_SQUISH "Build without squish." OFF)
OPTION(BUILD_MINI_WITH_OPENTHREADS "Build with openthreads." OFF)
OPTION(BUILD_MINI_WITH_GREYC "Build with GREYCstoration." OFF)

OPTION(BUILD_SHARED_LIBS "Build as a shared library." OFF)

# library name
IF (WIN32)
   SET(MINI_NAME "libMini")
   SET(MINISFX_NAME "libMiniSFX")
ELSE (WIN32)
   SET(MINI_NAME "Mini")
   SET(MINISFX_NAME "MiniSFX")
ENDIF (WIN32)

# path to libMini
IF (NOT LIBMINI_PATH)
   SET(LIBMINI_PATH $ENV{LIBMINI_PATH})
   IF (NOT LIBMINI_PATH)
      FIND_PATH(LIBMINI_PATH mini.h ${CMAKE_CURRENT_SOURCE_DIR}/../mini ${CMAKE_CURRENT_SOURCE_DIR}/../../libmini/mini)
   ENDIF(NOT LIBMINI_PATH)
ENDIF(NOT LIBMINI_PATH)

# default Unix compiler definitions
IF (NOT CMAKE_BUILD_TYPE)
   IF (UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O -finline-functions -Wall -Wno-parentheses")
   ENDIF (UNIX)
ENDIF (NOT CMAKE_BUILD_TYPE)

# build type
IF (CMAKE_BUILD_TYPE)
   STRING(TOUPPER ${CMAKE_BUILD_TYPE} MINI_BUILD_TYPE)
ELSE (CMAKE_BUILD_TYPE)
   SET(MINI_BUILD_TYPE DEFAULT)
ENDIF (CMAKE_BUILD_TYPE)
MESSAGE(STATUS MINI_BUILD_TYPE=${MINI_BUILD_TYPE})

# Windows compiler definitions
IF (WIN32)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4305")
ENDIF (WIN32)

# check environment variable for third party directory
IF (NOT LIBMINI_THIRDPARTY_DIR)
   SET(LIBMINI_THIRDPARTY_DIR $ENV{LIBMINI_THIRDPARTY_DIR})
ENDIF(NOT LIBMINI_THIRDPARTY_DIR)

MACRO(SET_MINI_PATH name subdir)
   SET(${name} ${LIBMINI_PATH}
               ${LIBMINI_PATH}/../deps/${subdir}
               ${LIBMINI_PATH}/../WIN32/${subdir}
               ${LIBMINI_PATH}/../../${subdir}
               /usr/local/${subdir})
   IF (LIBMINI_THIRDPARTY_DIR)
      SET(${name} ${${name}}
                  ${LIBMINI_THIRDPARTY_DIR}
                  ${LIBMINI_THIRDPARTY_DIR}/${subdir}
                  ${LIBMINI_THIRDPARTY_DIR}/deps/${subdir}
                  ${LIBMINI_THIRDPARTY_DIR}/WIN32/${subdir}
                  /usr/local/${subdir})
   ENDIF (LIBMINI_THIRDPARTY_DIR)
ENDMACRO(SET_MINI_PATH)

# paths to optional dependencies
SET_MINI_PATH(PTHREAD_PATH pthread)
SET_MINI_PATH(OPENTHREADS_PATH openthreads)
SET_MINI_PATH(CURL_PATH libcurl)
SET_MINI_PATH(JPEG_PATH libjpeg)
SET_MINI_PATH(PNG_PATH libpng)
SET_MINI_PATH(ZLIB_PATH zlib)
SET_MINI_PATH(SQUISH_PATH squish)
SET_MINI_PATH(GREYCSTORATION_PATH greycstoration)

# paths to optional WIN32 dependencies
SET_MINI_PATH(WIN32_PTHREAD_PATH pthreads-win32)
SET_MINI_PATH(WIN32_OPENTHREADS_PATH openthreads)
SET_MINI_PATH(WIN32_CURL_PATH libcurl)
SET_MINI_PATH(WIN32_JPEG_PATH libjpeg)
SET_MINI_PATH(WIN32_PNG_PATH libpng)
SET_MINI_PATH(WIN32_ZLIB_PATH zlib)
SET_MINI_PATH(WIN32_SQUISH_PATH squish)
SET_MINI_PATH(WIN32_FREEGLUT_PATH freeglut)

MACRO(FIND_MINI_LIBRARY name file path)
   IF (NOT ${name})
      IF (NOT MINI_BUILD_TYPE MATCHES DEBUG)
         FIND_LIBRARY(${name} ${file} PATHS ${path} PATH_SUFFIXES lib release minsizerel relwithdebinfo)
      ELSE (NOT MINI_BUILD_TYPE MATCHES DEBUG)
         FIND_LIBRARY(${name} NAMES ${file}d ${file} PATHS ${path} PATH_SUFFIXES lib debug)
      ENDIF (NOT MINI_BUILD_TYPE MATCHES DEBUG)
   ENDIF (NOT ${name})
ENDMACRO(FIND_MINI_LIBRARY)

MACRO(FIND_MINI_LIBRARY2 name file path1 path2)
   FIND_MINI_LIBRARY(name file path1)
   FIND_MINI_LIBRARY(name file path2)
ENDMACRO(FIND_MINI_LIBRARY2)

MACRO(FIND_MINI_PATH name file path)
   IF (NOT ${name})
      FIND_PATH(${name} ${file} PATHS ${path} PATH_SUFFIXES include)
   ENDIF (NOT ${name})
ENDMACRO(FIND_MINI_PATH)

MACRO(FIND_MINI_PATH2 name file path1 path2)
   FIND_MINI_PATH(name file path1)
   FIND_MINI_PATH(name file path2)
ENDMACRO(FIND_MINI_PATH2)

IF (NOT BUILD_MINI_STUB)

   # find OpenGL dependency
   FIND_PACKAGE(OpenGL)

   # find POSIX threads dependency
   IF (NOT BUILD_MINI_WITH_OPENTHREADS)
      FIND_MINI_LIBRARY2(PTHREAD_LIBRARY pthread "/usr" "${PTHREAD_PATH}")
      FIND_MINI_LIBRARY(PTHREAD_LIBRARY pthread_static "${WIN32_PTHREAD_PATH}")
      FIND_MINI_PATH(PTHREAD_INCLUDE_DIR pthread.h "/usr")
      FIND_MINI_PATH2(PTHREAD_INCLUDE_DIR pthread.h "${PTHREAD_PATH}" "${WIN32_PTHREAD_PATH}")
      INCLUDE_DIRECTORIES(${PTHREAD_INCLUDE_DIR})
      IF (WIN32)
         ADD_DEFINITIONS(-DPTW32_STATIC_LIB)
      ENDIF (WIN32)
      SET(THREAD_LIBRARY ${PTHREAD_LIBRARY})
   ELSE (NOT BUILD_MINI_WITH_OPENTHREADS)
      FIND_MINI_LIBRARY(OPENTHREADS_LIBRARY OpenThreads "${OPENTHREADS_PATH}")
      FIND_MINI_LIBRARY(OPENTHREADS_LIBRARY openthreads_static "${WIN32_OPENTHREADS_PATH}")
      FIND_MINI_PATH2(OPENTHREADS_INCLUDE_DIR OpenThreads/Thread "${OPENTHREADS_PATH}" "${WIN32_OPENTHREADS_PATH}")
      INCLUDE_DIRECTORIES(${OPENTHREADS_INCLUDE_DIR})
      IF (WIN32)
         ADD_DEFINITIONS(-DOT_LIBRARY_STATIC)
      ENDIF (WIN32)
      ADD_DEFINITIONS(-DUSEOPENTH)
      SET(THREAD_LIBRARY ${OPENTHREADS_LIBRARY})
   ENDIF (NOT BUILD_MINI_WITH_OPENTHREADS)

   # find CURL dependency
   FIND_MINI_LIBRARY(CURL_LIBRARY curl "${CURL_PATH}")
   FIND_MINI_LIBRARY(CURL_LIBRARY curllib_static "${WIN32_CURL_PATH}")
   FIND_MINI_PATH2(CURL_INCLUDE_DIR curl/curl.h "${CURL_PATH}" "${WIN32_CURL_PATH}")
   INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
   IF (WIN32)
      ADD_DEFINITIONS(-DCURL_STATICLIB)
   ENDIF (WIN32)

   # find JPEG dependency
   FIND_MINI_LIBRARY(JPEG_LIBRARY jpeg "${JPEG_PATH}")
   FIND_MINI_LIBRARY(JPEG_LIBRARY libjpeg "${WIN32_JPEG_PATH}")
   FIND_MINI_PATH2(JPEG_INCLUDE_DIR jpeglib.h "${JPEG_PATH}" "${WIN32_JPEG_PATH}")
   INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})

   # find PNG dependency
   FIND_MINI_LIBRARY(PNG_LIBRARY png "${PNG_PATH}")
   FIND_MINI_LIBRARY(PNG_LIBRARY libpng "${WIN32_PNG_PATH}")
   FIND_MINI_PATH2(PNG_INCLUDE_DIR png.h "${PNG_PATH}" "${WIN32_PNG_PATH}")
   INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})

   # find ZLIB dependency
   FIND_MINI_LIBRARY(ZLIB_LIBRARY z "${ZLIB_PATH}")
   FIND_MINI_LIBRARY(ZLIB_LIBRARY zlib "${WIN32_ZLIB_PATH}")
   FIND_MINI_PATH2(ZLIB_INCLUDE_DIR zlib.h "${ZLIB_PATH}" "${WIN32_ZLIB_PATH}")
   INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})

   # find Squish dependency
   FIND_MINI_LIBRARY2(SQUISH_LIBRARY squish "${SQUISH_PATH}" "${WIN32_SQUISH_PATH}")
   FIND_MINI_PATH2(SQUISH_INCLUDE_DIR squish.h "${SQUISH_PATH}" "${WIN32_SQUISH_PATH}")
   INCLUDE_DIRECTORIES(${SQUISH_INCLUDE_DIR})

   # find GREYCstoration dependency
   IF (BUILD_MINI_WITH_GREYC)
      FIND_MINI_PATH(GREYCSTORATION_INCLUDE_DIR CImg.h "${GREYCSTORATION_PATH}")
      IF (GREYCSTORATION_INCLUDE_DIR)
         INCLUDE_DIRECTORIES(${GREYCSTORATION_INCLUDE_DIR})
         ADD_DEFINITIONS(-DUSEGREYC)
      ENDIF (GREYCSTORATION_INCLUDE_DIR)
   ENDIF (BUILD_MINI_WITH_GREYC)

   # find GLUT dependency
   IF (WIN32)
      FIND_MINI_LIBRARY(GLUT_LIBRARY freeglut_static "${WIN32_FREEGLUT_PATH}")
      FIND_MINI_PATH(GLUT_INCLUDE_DIR GL/glut.h "${WIN32_FREEGLUT_PATH}")
   ELSE (WIN32)
      FIND_PACKAGE(GLUT)
   ENDIF (WIN32)
   INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
   IF (WIN32)
      ADD_DEFINITIONS(-DFREEGLUT_STATIC)
   ENDIF (WIN32)

ENDIF (NOT BUILD_MINI_STUB)

# find libMini dependency
FIND_MINI_LIBRARY(MINI_LIBRARY ${MINI_NAME} ${LIBMINI_PATH})
IF (NOT MINI_LIBRARY)
   SET(MINI_LIBRARY ${MINI_NAME})
ENDIF (NOT MINI_LIBRARY)
FIND_MINI_PATH(MINI_INCLUDE_DIR mini.h ${LIBMINI_PATH})
INCLUDE_DIRECTORIES(${MINI_INCLUDE_DIR}/..)

# find libMiniSFX dependency
FIND_MINI_LIBRARY(MINISFX_LIBRARY ${MINISFX_NAME} ${LIBMINI_PATH})
IF (NOT MINISFX_LIBRARY)
   SET(MINISFX_LIBRARY ${MINISFX_NAME})
ENDIF (NOT MINISFX_LIBRARY)
FIND_MINI_PATH(MINISFX_INCLUDE_DIR viewerbase.h ${LIBMINI_PATH})
INCLUDE_DIRECTORIES(${MINISFX_INCLUDE_DIR}/..)

# check for debug build
IF (MINI_BUILD_TYPE MATCHES DEBUG)
   ADD_DEFINITIONS(-DLIBMINI_DEBUG)
ENDIF (MINI_BUILD_TYPE MATCHES DEBUG)

# check for release build
IF (MINI_BUILD_TYPE MATCHES RELEASE)
   ADD_DEFINITIONS(-DLIBMINI_RELEASE)
ENDIF (MINI_BUILD_TYPE MATCHES RELEASE)

# check for stub build
IF (BUILD_MINI_STUB)
   ADD_DEFINITIONS(-DNOOGL)
   SET(OPENGL_LIBRARIES "")
ENDIF (BUILD_MINI_STUB)

# check for squish build
IF (BUILD_MINI_WITHOUT_SQUISH)
   ADD_DEFINITIONS(-DNOSQUISH)
   SET(SQUISH_LIBRARY "")
ENDIF (BUILD_MINI_WITHOUT_SQUISH)

# include libMini source list
INCLUDE(${LIBMINI_PATH}/libMini.cmake)
