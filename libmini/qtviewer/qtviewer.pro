TARGET = qtviewer
TEMPLATE = app

include(sources.pro)

!include(mini/sources.pro) {
   error("couldn't find libMini sources!")
}

!include(grid/sources.pro) {
   error("couldn't find libGrid sources!")
}

DEPENDPATH += mini
DEPENDPATH += grid

QT += core gui opengl
CONFIG += debug_and_release

CONFIG(debug, debug|release) {
   unix:TARGET = $$join(TARGET,,,_debug)
}

MOC_DIR = mocs
OBJECTS_DIR = objs
RCC_DIR = rccs
UI_DIR = uics

CONFIG(debug, debug|release) {
   unix:MOC_DIR = $$join(MOC_DIR,,,_debug)
   unix:OBJECTS_DIR = $$join(OBJECTS_DIR,,,_debug)
   unix:RCC_DIR = $$join(RCC_DIR,,,_debug)
   unix:UI_DIR = $$join(UI_DIR,,,_debug)
   win32:MOC_DIR = $$join(MOC_DIR,,,d)
   win32:OBJECTS_DIR = $$join(OBJECTS_DIR,,,d)
   win32:RCC_DIR = $$join(RCC_DIR,,,d)
   win32:UI_DIR = $$join(UI_DIR,,,d)
}

INCLUDEPATH += .
INCLUDEPATH += ..
INCLUDEPATH += ../..
INCLUDEPATH += ../include
INCLUDEPATH += ../../include
INCLUDEPATH += ../../libcurl/include
INCLUDEPATH += ../../squish/include
!mac:INCLUDEPATH += ../../libjpeg/include
INCLUDEPATH += ../../libpng/include
INCLUDEPATH += ../../zlib/include
INCLUDEPATH += ../../gdal/include

win32:INCLUDEPATH += ../WIN32
win32:INCLUDEPATH += ../WIN32/pthreads-win32
win32:INCLUDEPATH += ../WIN32/libcurl
win32:INCLUDEPATH += ../WIN32/squish
win32:INCLUDEPATH += ../WIN32/libjpeg
win32:INCLUDEPATH += ../WIN32/libpng
win32:INCLUDEPATH += ../WIN32/zlib

LIBS += -L.
LIBS += -L..
LIBS += -L../..
LIBS += -L../lib
LIBS += -L../../lib
LIBS += -L../../libcurl/lib
LIBS += -L../../squish/lib
!mac:LIBS += -L../../libjpeg/lib
LIBS += -L../../libpng/lib
LIBS += -L../../zlib/lib
LIBS += -L../../gdal/lib

win32:LIBS += -L../WIN32
win32:LIBS += -L../WIN32/pthreads-win32
win32:LIBS += -L../WIN32/libcurl
win32:LIBS += -L../WIN32/squish
win32:LIBS += -L../WIN32/libjpeg
win32:LIBS += -L../WIN32/libpng
win32:LIBS += -L../WIN32/zlib

unix:LIBS += -lsquish -lcurl -ljpeg -lpng -lz
unix:LIBS += -lgdal

win32:LIBS += -lsquish -lcurllib_static -llibjpeg -llibpng -lzlib -lpthread_static
win32:LIBS += -lgdal_i
win32:LIBS += -lws2_32 -lwinmm

RESOURCES += qtviewer.qrc

unix:QMAKE_CXXFLAGS += -O
unix:QMAKE_CXXFLAGS_WARN_ON += -w -Wall -Wno-parentheses

win32:DEFINES += _CRT_SECURE_NO_DEPRECATE
win32:DEFINES += PTW32_STATIC_LIB
win32:DEFINES += CURL_STATICLIB
win32:QMAKE_CXXFLAGS_WARN_ON += /wd4244 /wd4305

mac:ICON = qtviewer.icns
win32:RC_FILE = mini/libMini.rc
