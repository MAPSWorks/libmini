#!/bin/tcsh -f

# unix build script
# (c) by Stefan Roettger
#
# options:
#  release: build in release mode
#  debug: build in debug mode
#  clean: remove all build files
#  zip: make distribution

if ($1 == "clean") then
   make clean
   rm -rf Makefile* qtviewer qtviewer_debug qtviewer*.app mocs* objs* rccs* uics*
   exit
endif

if ($1 == "zip") then
   (cd ..; zip -qr9 qtviewer/QTVIEWER.zip qtviewer -x \*/.\?\?\* -x \*CVS/\*)
   exit
endif

set QTPATH=/usr/local/Trolltech/Qt-4.7.4/bin
if (-d $QTPATH) then
   setenv PATH $PATH":"$QTPATH
   rehash
endif

if (! -X qmake) then
   echo error: qmake is not installed!
   echo suggest: add your qmake binary path to your search path
   echo for example using tcsh: 'setenv PATH $PATH":/usr/local/Trolltech/Qt-4.7.4/bin"'
   exit 1
endif

set qoptions=""
if ($HOSTTYPE == "intel-pc") set qoptions="$qoptions -spec macx-g++"

set moptions=""
if ($1 != "") then
   if ($1 != "release" && $1 != "debug") then
      echo error: bad usage!
      exit 1
   endif
   set moptions="$moptions $1"
else
   set moptions="$moptions release"
endif
set moptions="$moptions -j4"

qmake $qoptions && make $moptions