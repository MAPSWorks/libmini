#!/bin/tcsh -f

# this script installs the libMini dependencies locally
# the installation path can be supplied as an optional argument (default is /usr/local)
# if the installation path begins with /usr, the script asks for the super user password

set arg=$1

if ($arg != "") then
   set prefix=$arg
else
   set prefix=/usr/local
endif

echo "installing to $prefix"

set sudo=""
if ($prefix:h == "/usr") then
   if ($USER != "root") set sudo="sudo"
endif

# zlib
if (-e zlib) then
   if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip zlib on MacOS X
      echo BUILDING ZLIB
      (cd zlib;\
       ./configure --prefix=$prefix/zlib;\
       make -j 2; $sudo make install)
   endif
endif

# libjpeg
if (-e libjpeg) then
   if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip libjpeg on MacOS X
      echo BUILDING LIBJPEG
      (cd libjpeg;\
       ./configure --prefix=$prefix/libjpeg --enable-static --disable-shared;\
       make -j 2; $sudo make install)
   endif
endif

# libpng
if (-e libpng) then
   echo BUILDING LIBPNG
   (cd libpng;\
    ./configure --prefix=$prefix/libpng --enable-static --disable-shared;\
    make -j 2; $sudo make install)
endif

# curl
if (-e libcurl) then
   echo BUILDING CURL
   (cd libcurl;\
    ./buildconf;\
    ./configure --prefix=$prefix/libcurl\
                --without-ssl --disable-ldap --disable-ldaps\
                --enable-static --disable-shared;\
    make -j 2; $sudo make install)
endif

# squish
if (-e squish) then
   echo BUILDING SQUISH
   (cd squish;\
    cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish;\
    make -j 2; $sudo make install)
endif

# freeglut
if (-e freeglut) then
   if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
      echo BUILDING FREEGLUT
      (cd freeglut;\
       cmake -DCMAKE_INSTALL_PREFIX=$prefix/freeglut;\
       make -j 2; $sudo make install)
   endif
endif

# other libraries to install:

# proj (required by gdal)
if (-e proj) then
   echo BUILDING PROJ.4
   (cd proj;\
    ./configure --prefix=$prefix;\
    make -j 2; $sudo make install)
endif

# libiconv (required by gdal 1.9+)
if (-e libiconv) then
   echo BUILDING LIBICONV
   (cd libiconv;\
    ./configure --prefix=$prefix/libiconv --enable-static --disable-shared;\
    make -j 2; $sudo make install)
endif

# gdal
if (-e gdal) then
   echo BUILDING GDAL
   (cd gdal;\
    ./configure --prefix=$prefix/gdal\
                --with-libtiff=internal --with-geotiff=internal\
                --with-png=internal --with-jpeg=internal\
                --without-curl --without-netcdf\
                --without-sqlite3 --without-pg\
                --without-ld-shared\
                --enable-static --disable-shared;\
    make -j 2; $sudo make install)
endif

# dcmtk
if (-e dcmtk) then
   echo BUILDING DCMTK
   if ($HOSTTYPE != "intel-mac" && $HOSTTYPE != "intel-pc") then
      (cd dcmtk;\
       ./configure --prefix=$prefix/dcmtk CXXFLAGS="-g -O2 -fpermissive";\
       make -j 2; $sudo make install-lib)
   else
      (cd dcmtk;\
       ./configure --prefix=$prefix/dcmtk;\
       make -j 2; $sudo make install-lib)
   endif
endif

# sqlite amalgamation
if (-e sqlite-amalgamation) then
   echo "BUILDING SQLite (amalgamation)"
   (cd sqlite-amalgamation;\
    gcc -DSQLITE_ENABLE_RTREE=1 shell.c sqlite3.c -lpthread -ldl -o sqlite3)
endif

# sqlite autoconf
if (-e sqlite-autoconf) then
   echo "BUILDING SQLite (autoconf)"
   (cd sqlite-autoconf;\
    ./configure --prefix=$prefix --disable-shared --enable-static;\
    make -j 2; $sudo make install)
endif
