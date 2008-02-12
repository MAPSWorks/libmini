#!/bin/tcsh -f

if ($1 == "" || $2 == "") then
   echo "usage: $0 input.tif output.tif"
   echo "   output.tif receives the geotag of input.tif"
   echo "   black (0,0,0) is replaced with (0,0,1)"
endif

set in=$1
set out=$2

if ($in:e != "tif") exit
if ($out:e != "tif") exit

convert -fill #000001 -opaque #000000 $in $out
modgeotiff.sh $in $out
