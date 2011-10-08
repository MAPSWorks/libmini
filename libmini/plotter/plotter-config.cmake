# cmake build configuration for libMini plotter
# (c) by Stefan Roettger

# library name
IF (WIN32)
   SET(MINI_NAME "libMini")
   SET(PLOTTER_NAME "libPlot")
ELSE (WIN32)
   SET(MINI_NAME "Mini")
   SET(PLOTTER_NAME "Plot")
ENDIF (WIN32)

# path to libMini
IF (NOT LIBMINI_PATH)
   SET(LIBMINI_PATH $ENV{LIBMINI_PATH})
   IF (NOT LIBMINI_PATH)
      FIND_PATH(LIBMINI_PATH mini.h ${CMAKE_CURRENT_SOURCE_DIR}/mini ${CMAKE_CURRENT_SOURCE_DIR}/../mini ${CMAKE_CURRENT_SOURCE_DIR}/../../libmini/mini)
   ENDIF (NOT LIBMINI_PATH)
ENDIF (NOT LIBMINI_PATH)

# path to plotter
IF (NOT PLOTTER_PATH)
   SET(PLOTTER_PATH $ENV{PLOTTER_PATH})
   IF (NOT PLOTTER_PATH)
      FIND_PATH(PLOTTER_PATH plot.h ${CMAKE_CURRENT_SOURCE_DIR}/../plotter ${CMAKE_CURRENT_SOURCE_DIR}/../../libmini/plotter)
   ENDIF (NOT PLOTTER_PATH)
ENDIF (NOT PLOTTER_PATH)

# default Unix compiler definitions
IF (NOT CMAKE_BUILD_TYPE)
   IF (UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O -finline-functions -Wall -Wno-parentheses")
   ENDIF (UNIX)
ENDIF (NOT CMAKE_BUILD_TYPE)

# build type
IF (CMAKE_BUILD_TYPE)
   STRING(TOUPPER ${CMAKE_BUILD_TYPE} PLOTTER_BUILD_TYPE)
ELSE (CMAKE_BUILD_TYPE)
   SET(PLOTTER_BUILD_TYPE DEFAULT)
ENDIF (CMAKE_BUILD_TYPE)
MESSAGE(STATUS PLOTTER_BUILD_TYPE=${PLOTTER_BUILD_TYPE})

# Windows compiler definitions
IF (MSVC)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4305")
ENDIF (MSVC)

# check environment variable for third party directory
IF (NOT PLOTTER_THIRDPARTY_DIR)
   SET(PLOTTER_THIRDPARTY_DIR $ENV{PLOTTER_THIRDPARTY_DIR})
ENDIF (NOT PLOTTER_THIRDPARTY_DIR)

MACRO(SET_PLOTTER_PATH name subdir)
   SET(${name} ${PLOTTER_PATH}
               ${PLOTTER_PATH}/${subdir}
               ${PLOTTER_PATH}/../${subdir}
               ${PLOTTER_PATH}/../deps/${subdir}
               ${PLOTTER_PATH}/../WIN32/${subdir}
               ${PLOTTER_PATH}/../../${subdir}
               /usr/local/${subdir})
   IF (PLOTTER_THIRDPARTY_DIR)
      SET(${name} ${${name}}
                  ${PLOTTER_THIRDPARTY_DIR}
                  ${PLOTTER_THIRDPARTY_DIR}/${subdir}
                  ${PLOTTER_THIRDPARTY_DIR}/deps/${subdir}
                  ${PLOTTER_THIRDPARTY_DIR}/WIN32/${subdir}
                  /usr/local/${subdir})
   ENDIF (PLOTTER_THIRDPARTY_DIR)
ENDMACRO(SET_PLOTTER_PATH)

# paths to optional dependencies
SET_PLOTTER_PATH(FREEGLUT_PATH freeglut)

# paths to optional WIN32 dependencies
SET_PLOTTER_PATH(WIN32_FREEGLUT_PATH freeglut)

MACRO(FIND_PLOTTER_LIBRARY name file path)
   IF (NOT ${name})
      IF (NOT PLOTTER_BUILD_TYPE MATCHES DEBUG)
         FIND_LIBRARY(${name} ${file} PATHS ${path} PATH_SUFFIXES lib release minsizerel relwithdebinfo)
      ELSE (NOT PLOTTER_BUILD_TYPE MATCHES DEBUG)
         FIND_LIBRARY(${name} NAMES ${file}d ${file} PATHS ${path} PATH_SUFFIXES lib debug)
      ENDIF (NOT PLOTTER_BUILD_TYPE MATCHES DEBUG)
   ENDIF (NOT ${name})
ENDMACRO(FIND_PLOTTER_LIBRARY)

MACRO(FIND_PLOTTER_PATH name file path)
   IF (NOT ${name})
      FIND_PATH(${name} ${file} PATHS ${path} PATH_SUFFIXES include)
   ENDIF (NOT ${name})
ENDMACRO(FIND_PLOTTER_PATH)

# find libMini dependency
FIND_PLOTTER_LIBRARY(MINI_LIBRARY ${MINI_NAME} ${LIBMINI_PATH})
IF (NOT MINI_LIBRARY)
   SET(MINI_LIBRARY ${MINI_NAME})
ENDIF (NOT MINI_LIBRARY)
FIND_PLOTTER_PATH(MINI_INCLUDE_DIR mini.h ${LIBMINI_PATH})
INCLUDE_DIRECTORIES(${MINI_INCLUDE_DIR}/..)

# find OpenGL dependency
FIND_PACKAGE(OpenGL)
IF (NOT OPENGL_LIBRARIES)
   SET(OPENGL_LIBRARIES ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
ENDIF (NOT OPENGL_LIBRARIES)

# find GLUT dependency
IF (WIN32)
   FIND_PLOTTER_LIBRARY(GLUT_LIBRARY freeglut_static "${WIN32_FREEGLUT_PATH}")
   FIND_PLOTTER_PATH(GLUT_INCLUDE_DIR GL/glut.h "${WIN32_FREEGLUT_PATH}")
ELSE (WIN32)
   FIND_PACKAGE(GLUT)
   IF (NOT GLUT_LIBRARY OR NOT GLUT_INCLUDE_DIR)
      FIND_PLOTTER_LIBRARY(GLUT_LIBRARY glut "${FREEGLUT_PATH}")
      FIND_PLOTTER_PATH(GLUT_INCLUDE_DIR GL/glut.h "${FREEGLUT_PATH}")
   ENDIF (NOT GLUT_LIBRARY OR NOT GLUT_INCLUDE_DIR)
   IF (NOT GLUT_LIBRARY)
      SET(GLUT_LIBRARY glut)
   ENDIF (NOT GLUT_LIBRARY)
   IF (NOT GLUT_INCLUDE_DIR)
      SET(GLUT_INCLUDE_DIR /usr/include/GL)
   ENDIF (NOT GLUT_INCLUDE_DIR)
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
IF (WIN32)
   ADD_DEFINITIONS(-DFREEGLUT_STATIC)
ENDIF (WIN32)

# check for debug build
IF (PLOTTER_BUILD_TYPE MATCHES DEBUG)
   ADD_DEFINITIONS(-DPLOTTER_DEBUG)
ENDIF (PLOTTER_BUILD_TYPE MATCHES DEBUG)

# check for release build
IF (PLOTTER_BUILD_TYPE MATCHES RELEASE)
   ADD_DEFINITIONS(-DPLOTTER_RELEASE)
ENDIF (PLOTTER_BUILD_TYPE MATCHES RELEASE)

MACRO(MAKE_PLOTTER_EXECUTABLE name)
   ADD_EXECUTABLE(${name} ${name}.cpp)
   TARGET_LINK_LIBRARIES(${name}
      ${PLOTTER_NAME}
      ${MINI_LIBRARY}
      ${OPENGL_LIBRARIES}
      ${GLUT_LIBRARY}
      )
ENDMACRO(MAKE_PLOTTER_EXECUTABLE)
