#!/bin/tcsh -f

# this script installs the libMini dependencies locally
# the installation path can be supplied as an optional argument (default is /usr/local)

set arg=$1

if ($arg != "") then
   set prefix=$arg
else
   set prefix=/usr/local
endif

echo "installing to $prefix"

# libjpeg
if (-e libjpeg) then
   echo BUILDING LIBJPEG
   (cd libjpeg; ./configure --prefix=$prefix/libjpeg --enable-static --disable-shared; make -j 2; make install)
endif

# libpng
if (-e libpng) then
   echo BUILDING LIBPNG
   (cd libpng; ./configure --prefix=$prefix/libpng --enable-static --disable-shared; make -j 2; make install)
endif

# zlib
if (-e zlib) then
   echo BUILDING ZLIB
   (cd zlib; ./configure --prefix=$prefix/zlib; make -j 2; make install)
endif

# curl
if (-e libcurl) then
   echo BUILDING CURL
   (cd libcurl;\
    ./buildconf;\
    ./configure --prefix=$prefix/libcurl\
                --without-ssl --disable-ldap --disable-ldaps\
                --enable-static --disable-shared;\
    make -j 2; make install)
endif

# squish
echo BUILDING SQUISH
(cd squish; cmake -DCMAKE_INSTALL_PREFIX=$prefix/squish; make -j 2; make install)

# freeglut
if ($HOSTTYPE != "intel-pc" && $HOSTTYPE != "intel-mac") then # skip freeglut on MacOS X
   echo BUILDING FREEGLUT
   (cd freeglut; ./autogen.sh; ./autogen.sh; ./configure --prefix=$prefix/freeglut --enable-static --disable-shared; make -j 2; make install)
endif

# other libraries to install:

# gdal
echo BUILDING GDAL
(cd gdal;\
 ./configure --prefix=$prefix/gdal\
             --with-libtiff=internal --with-geotiff=internal\
             --with-rename-internal-libtiff-symbols\
             --with-png=internal --with-jpeg=internal\
             --without-threads --without-curl --without-netcdf\
             --without-sqlite3 --without-pg\
             --without-ld-shared --with-static-proj4\
             --enable-static --disable-shared;\
 make -j 2; make install)

# libiconv (required by gdal 1.9+)
if (-e libiconv) then
   echo BUILDING LIBICONV
   (cd libiconv; ./configure --prefix=$prefix/libiconv --enable-static --disable-shared; make -j 2; make install)
endif
