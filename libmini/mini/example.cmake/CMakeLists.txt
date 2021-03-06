# cmake build file

PROJECT(MyApp)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

# dependencies

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")

FIND_PACKAGE(MINI)
INCLUDE_DIRECTORIES(${GRID_INCLUDE_DIR})

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(GLUT)

# header and module list

SET(LIB_HDRS
   # module headers
   module.h
   )

SET(LIB_SRCS
   # module sources
   module.cpp
   )

# library

SET(LIB_NAME ${PROJECT_NAME})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(${LIB_NAME} ${LIB_SRCS} ${LIB_HDRS})

# executable

ADD_EXECUTABLE(example example.cpp)
TARGET_LINK_LIBRARIES(example
   ${LIB_NAME}
   ${MINI_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${GLUT_LIBRARIES}
   )
